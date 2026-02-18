#pragma once
#include "../util/Option.h"
#include "aabb.h"
#include "../world/voxeltraversal.h"
namespace collision {

	struct HitQuery {
		stn::Option<ecs::obj> orgin;
		explicit HitQuery(ecs::Ecs& ecs) :orgin(stn::None), world(ecs) {
		}
		ecs::Ecs& world;
		explicit HitQuery(const ecs::obj& orgin_obj) : orgin(orgin_obj), world(orgin.unwrap().world()) {
		}
		bool matches(aabb::Collider& collider) {
			return collider.owner() == orgin;
		}
	};

	inline voxtra::WorldRayCollision raycast_dynamic(math::ray search_ray, HitQuery query) {
		ecs::View<ecs::With<aabb::Collider>, ecs::With<aabb::DynamicCollider>> colliders(query.world);
		voxtra::WorldRayCollision closest = stn::None;
		for (auto [collider, dynamic_tag] : colliders) {
			static_assert(std::same_as<aabb::Collider&,decltype(collider)>);
			if (query.matches(collider)) {
				continue;
			}
			if (search_ray.point_lies_in_sphere(collider.global_box().center)) {
				geointersect::boxRayCollision blkinter = geointersect::intersection(collider.global_box(), search_ray);
				stn::Option<float> test_dist = blkinter.member(&geointersect::RayHit::dist);
				stn::Option<float> current_dist = closest.member_ref(&voxtra::RayWorldHit::Hit).member(&geointersect::RayHit::dist);
				if (test_dist < current_dist) {
					closest = voxtra::RayWorldHit(blkinter.unwrap(), collider);
				}
			}
		}
		return closest;
	}
	inline voxtra::WorldRayCollision raycast(math::ray nray, HitQuery query, voxtra::GridTraverseMode travmode=voxtra::GridTraverseMode::countnormal) {
		voxtra::WorldRayCollision closest_on_grid = voxtra::travvox(nray, travmode);
		voxtra::WorldRayCollision closest_entity = raycast_dynamic(nray, query);
		return stn::min_some_on_map(closest_entity, closest_on_grid,
		[&](const voxtra::RayWorldHit& col) {return col.dist(); });
	}
}