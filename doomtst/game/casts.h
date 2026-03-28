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
				geointersect::boxRayCollision blkinter = geointersect::intersection(collider.global_box(), search_ray);
				stn::Option<double> test_dist = blkinter.map_member(&geointersect::RayHit::length);
				stn::Option<double> current_dist = closest.map_member(&voxtra::RayWorldHit::dist);
				if (test_dist.unwrap_or(std::numeric_limits<double>().infinity()) < current_dist.unwrap_or(std::numeric_limits<double>().infinity())) {
					if (test_dist.unwrap()<=search_ray.length()) {
						closest = voxtra::RayWorldHit(blkinter.unwrap(), collider);
					}
				}
			}
		return closest;
	}
	inline voxtra::WorldRayCollision raycast(math::ray nray, HitQuery query, voxtra::GridTraverseMode travmode=voxtra::GridTraverseMode::countnormal) {
		voxtra::WorldRayCollision closest_on_grid = voxtra::grid_cast(nray, travmode,query.world.get_resource<grid::Grid>());
		voxtra::WorldRayCollision closest_entity = raycast_dynamic(nray, query);
		return stn::min_some_on_map(closest_entity, closest_on_grid,
		[&](const voxtra::RayWorldHit& col) {return col.dist(); });
	}
}