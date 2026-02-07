#include "aabb.h"
#include "entity.h"
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
#include "../util/Option.h"
using namespace aabb;
#pragma once 
namespace collision {
	struct collision_event {
		template<ecs::ComponentType T>
		bool source_has_component() {
			return source.has_component<T>();
		}
		template<ecs::ComponentType T>
		T& source_get_component() {
			return source.get_component<T>();
		}
		template<ecs::ComponentType T>
		bool target_has_component() {
			return target.has_component<T>();
		}
		template<ecs::ComponentType T>
		T& target_get_component() {
			return target.get_component<T>();
		}
		//the target and source are abstrctions for dealing with events and for now each event wil lalso have a 
		ecs::obj target;
		ecs::obj source;

		collision_event(ecs::obj o1, ecs::obj o2) :source(o1), target(o2) {
		}
	};
	void write_collision_event(ecs::obj o1, ecs::obj o2) {
		o1.world().emplace_event<collision_event>(o1, o2);
		o2.world().emplace_event<collision_event>(o2, o1);
	}

	void distribute_collision_force(ecs::obj p1, ecs::obj p2, Vec3 force) {
		float p1_mass = p1.get_component_opt<rigidbody>().member(&rigidbody::mass).unwrap_or(0);
		float p2_mass = p2.get_component_opt<rigidbody>().member(&rigidbody::mass).unwrap_or(0);
		float total_mass = p1_mass + p2_mass;
		if (p1_mass) {
			v3::Vec3 p1_force = force * p2_mass / total_mass;
			p1.get_component<ecs::transform_comp>().transform.position += p1_force;
		}
		if (p2_mass) {
			v3::Vec3 p2_force = -force * p1_mass / total_mass;
			p2.get_component<ecs::transform_comp>().transform.position += p2_force;
		}
	}

	stn::Option<Vec3> colide_dynamic_static(Collider& dynamic_collider, Collider& static_collider, bool is_trigger) {
		Option<Vec3> force = aabb::collide_aabb(dynamic_collider, static_collider);
		if (!force) {
			return stn::None;
		}
		if (is_trigger) {
			collision::write_collision_event(dynamic_collider.owner(), static_collider.owner());
		}
		return force.retain(!static_collider.effector && !dynamic_collider.effector);
	}


	struct HitQuery {
		stn::Option<ecs::obj> orgin;
		explicit HitQuery(ecs::Ecs& ecs) :orgin(stn::None), world(ecs) {
		}
		ecs::Ecs& world;
		explicit HitQuery(const ecs::obj& orgin_obj) : orgin(orgin_obj), world(orgin.unwrap().world()) {
		}
		bool matches(Collider& collider) {
			return collider.owner() == orgin;
		}
	};


	struct DynamicCollisionSystem :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::View<DynamicCollider, Collider> colliders(world);
			for (auto&& [dynamic_tag_1, collider_1] : colliders) {
				for (auto&& [dynamic_tag_2, collider_2] : colliders) {
					if (collider_1.owner() != collider_2.owner()) {
						Option<v3::Vec3> force = aabb::collide_aabb(collider_1, collider_2);
						if (force.is_none()) {
							continue;
						}
						write_collision_event(collider_1.owner(), collider_2.owner());
						if (collider_1.effector || collider_2.effector) {
							continue;
						}
						distribute_collision_force(collider_1.owner(), collider_2.owner(), force.unwrap());
					}
				}
			}
		}
	};
	struct StaticCollsionSystem :ecs::System {
		void run(ecs::Ecs& world) {
			for (int iters = 0; iters < iterations; iters++) {
				ecs::View<DynamicCollider, Collider> colliders(world);
				for (auto&& [dynamic_tag, collider] : colliders) {
					geo::Box entity_box = collider.global_box();
					array<stn::non_null<block>> blocks = collider.world().get_resource<grid::Grid>().unwrap().voxel_in_range(entity_box);
					stn::Option<Vec3> min_force;
					stn::Option<block&> min_block;
					for (stn::non_null<block> block : blocks) {
						stn::Option<Collider&> aabb = block->owner().get_component_opt<Collider>();
						if (!aabb) {
							continue;
						}
						stn::Option<Vec3> force = colide_dynamic_static(collider, aabb.unwrap(), iters==0);
						if (min_force.map_member(&v3::Vec3::length) < force.map_member(&v3::Vec3::length)) {
							min_block = *block;
							min_force = force.unwrap();
						}
					}
					if (min_block && !collider.effector) {
						distribute_collision_force(collider.owner(), min_block.unwrap().owner(), min_force.unwrap());
					}
				}
			}
		}

		StaticCollsionSystem(size_t phycics_iterations) :iterations(phycics_iterations) {

		}
		size_t iterations = 0;
	};




	
	//casting
	inline bool boxcast_dynamic(geo::Box blk, HitQuery query) {
		ecs::View<Collider, DynamicCollider> colliders(query.world);
		for (auto [collider, dynamic_tag] : colliders) {
			if (query.matches(collider)) {
				continue;
			}
			if (box_intersects_aabb(blk, collider)) {
				return true;
			}

		}
		return false;
	}

	voxtra::WorldRayCollision raycast_dynamic(ray search_ray, HitQuery query) {
		ecs::View<Collider, DynamicCollider> colliders(query.world);
		voxtra::WorldRayCollision closest = stn::None;
		for (auto [collider, dynamic_tag] : colliders) {
			if (query.matches(collider)) {
				continue;
			}
			if (search_ray.point_lies_in_sphere(collider.global_box().center)) {
				geointersect::boxRayCollision blkinter = geointersect::intersection(collider.global_box(), search_ray);
				stn::Option<float> test_dist = blkinter.copy_member(&geointersect::RayHit::dist);
				stn::Option<float> current_dist = closest.member(&voxtra::RayWorldHit::Hit).member(&geointersect::RayHit::dist);
				if (test_dist < current_dist) {
					closest = voxtra::RayWorldHit(blkinter.unwrap(), collider);
				}
			}
		}
		return closest;
	}
	voxtra::WorldRayCollision raycast(ray nray, HitQuery query, voxtra::GridTraverseMode travmode) {
		voxtra::WorldRayCollision closest_on_grid = voxtra::travvox(nray, travmode);
		voxtra::WorldRayCollision closest_entity = raycast_dynamic(nray, query);
		if (closest_on_grid.map_member(&voxtra::RayWorldHit::dist) < closest_entity.map_member(&voxtra::RayWorldHit::dist)) {
			return closest_on_grid;
		}
		return closest_entity;
	}
}
