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

	inline voxtra::RayWorldCollision raycast_dynamic(geo::ray search_ray, HitQuery query) {
		ecs::View< aabb::Collider,aabb::DynamicCollider,ecs::Owner> colliders(query.world);
		voxtra::RayWorldCollision closest = stn::None;
		for (auto [collider, dynamic_tag,object] : colliders) {
			if (collider.effector) {
				continue;
			}
			if (query.matches(collider)) {
				continue;
			}
				geo::RayCollision blkinter = geo::intersection(collider.global_box(), search_ray);
				stn::Option<double> test_dist = blkinter.map_member(&geo::RayHit::length);
				stn::Option<double> current_dist = closest.map_member(&voxtra::RayWorldHit::dist);
				if (test_dist.unwrap_or(std::numeric_limits<double>().infinity()) < current_dist.unwrap_or(std::numeric_limits<double>().infinity())) {
						closest = voxtra::RayWorldHit(blkinter.unwrap(), object);
				}
			}
		return closest;
	}

	inline voxtra::RayWorldCollision raybox_cast_dynamic(geo::RayBox search_ray, HitQuery query) {
		ecs::View< aabb::Collider,  aabb::DynamicCollider,ecs::Owner> colliders(query.world);
		voxtra::RayWorldCollision closest = stn::None;
		for (auto [collider, dynamic_tag,object] : colliders) {
			if (collider.effector) {
				continue;
			}
			static_assert(std::same_as<aabb::Collider&, decltype(collider)>);
			if (query.matches(collider)) {
				continue;
			}
			geo::RayCollision blkinter = geo::intersection(search_ray,collider.global_box());
			stn::Option<double> test_dist = blkinter.map_member(&geo::RayHit::length);
			stn::Option<double> current_dist = closest.map_member(&voxtra::RayWorldHit::dist);
			if (test_dist.unwrap_or(std::numeric_limits<double>().infinity()) < current_dist.unwrap_or(std::numeric_limits<double>().infinity())) {
					closest = voxtra::RayWorldHit(blkinter.unwrap(), object);
			}
		}
		return closest;
	}
	inline voxtra::RayWorldCollision ray_box_cast(geo::RayBox ray_box, HitQuery query, voxtra::GridTraverseMode travmode = voxtra::GridTraverseMode::countnormal) {
		voxtra::RayWorldCollision closest_on_grid = voxtra::grid_ray_box_cast(ray_box, query.world.get_resource<grid::Grid>());
		voxtra::RayWorldCollision closest_entity = raybox_cast_dynamic(ray_box, query);
		return stn::min_some_on_map(closest_entity, closest_on_grid,
			[&](const voxtra::RayWorldHit& col) {return col.dist(); });
	}

	inline voxtra::RayWorldCollision raycast(geo::ray nray, HitQuery query, voxtra::GridTraverseMode travmode=voxtra::GridTraverseMode::countnormal) {
		voxtra::RayWorldCollision closest_on_grid = voxtra::grid_cast(nray, travmode,query.world.get_resource<grid::Grid>());
		voxtra::RayWorldCollision closest_entity = raycast_dynamic(nray, query);
		return stn::min_some_on_map(closest_entity, closest_on_grid,
		[&](const voxtra::RayWorldHit& col) {return col.dist(); });
	}
}