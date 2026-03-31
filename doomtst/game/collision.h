#include "aabb.h"

#include "../world/grid.h"
#include "../world/voxeltraversal.h"
#include "../util/Option.h"
#include "rigidbody.h"
#include "Core.h"
using namespace aabb;
#pragma once 
namespace collision {
	struct collision_event {
		//the target and source are abstrctions for dealing with events and for now each event wil lalso have a 
		ecs::obj target;
		ecs::obj source;

		collision_event(ecs::obj o1, ecs::obj o2) :source(o1), target(o2) {
		}
	};
	inline void write_collision_event(ecs::obj o1, ecs::obj o2) {
		o1.world().emplace_event<collision_event>(o1, o2);
		o2.world().emplace_event<collision_event>(o2, o1);
	}

	inline void distribute_collision_force(ecs::obj p1, ecs::obj p2, Vec3 force) {
		stn::Option<float> p1_mass = p1.get_component_opt<physics::rigidbody>().member(&physics::rigidbody::mass);
		stn::Option<float> p2_mass = p2.get_component_opt<physics::rigidbody>().member(&physics::rigidbody::mass);
		double total_mass = p1_mass.unwrap_or(0) + p2_mass.unwrap_or(0);
		if (force.mag2() > 1) {
			int l = 3;
		}
		if (p1_mass) {
			v3::Vec3 p1_force = force * p2_mass.unwrap_or(total_mass) / total_mass;
			p1.get_component<ecs::world_transform>().transform.position += p1_force;
		}
		if (p2_mass) {
			v3::Vec3 p2_force = -force * p1_mass.unwrap_or(total_mass) / total_mass;
			p2.get_component<ecs::world_transform>().transform.position += p2_force;
		}
	}




	struct DynamicCollisionSystem :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::View< DynamicCollider,Collider, ecs::Owner> colliders(world);
			for (auto&& [dynamic_tag_1, collider_1, obj_1] : colliders) {
				for (auto&& [dynamic_tag_2, collider_2, obj_2] : colliders) {
					if (obj_1 != obj_2) {
						Option<v3::Vec3> force = aabb::collide_aabb(collider_1, collider_2);
						if (force.is_none()) {
							continue;
						}
						write_collision_event(obj_1, obj_2);
						if (collider_1.effector || collider_2.effector) {
							continue;
						}
					}
				}
			}
		}
	};
	struct StaticCollsionSystem :ecs::System {

		void run(ecs::Ecs& world) {
			ecs::View<DynamicCollider,Collider,ecs::Owner> colliders(world);
			for (auto&& [dynamic_tag, collider, object] : colliders) {
				geo::Box entity_box = collider.global_box().expanded(v3::Scale3() / 100.0f);
				array<Chunks::block_object> blocks = collider.world().get_resource<grid::Grid>().voxel_in_range(entity_box);
				for (Chunks::block_object& block : blocks) {
					stn::Option<Collider&> aabb = block.get_component_opt<Collider>();
					if (!aabb) {
						continue;
					}
					Option<Vec3> force = aabb::collide_aabb(aabb.unwrap(), collider);
					if (!force) {
						collision::write_collision_event(block.object(), object);
					}
				}
			}
		}
	};

	struct CollsionPlugin :Core::Plugin {
		void build(Core::App& app) {
			app.emplace_system< StaticCollsionSystem>();
			app.emplace_system<DynamicCollisionSystem>();

		}
	};



	//casting
	inline bool boxcast_dynamic(geo::Box blk, HitQuery query) {
		ecs::View< Collider,DynamicCollider> colliders(query.world);
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
	inline bool boxcast(geo::Box box, HitQuery query) {
		return voxtra::boxcast_grid(box, query.world.get_resource<grid::Grid>()) || boxcast_dynamic(box, query);
	}

}