#pragma once
#include "../util/Option.h"
#include "aabb.h"
#include "../world/voxeltraversal.h"
namespace collision {

	template<collision::ObjectPredicate U=HitQuery>
	inline collision::RayWorldCollision raycast_dynamic(geo::ray search_ray, ecs::Ecs& world, U query= U()) {
		ecs::View< collision::Collider,collision::DynamicCollider,ecs::Owner> colliders(world);
		collision::RayWorldCollision closest = stn::None;
		for (auto [collider, dynamic_tag,object] : colliders) {
			if (collider.effector) {
				continue;
			}
			if (!query(object)) {
				continue;
			}
				geo::RayCollision blkinter = geo::intersection(collision::global_box(object), search_ray);
				stn::Option<double> test_dist = blkinter.map_member(&geo::RayHit::length);
				stn::Option<double> current_dist = closest.map_member(&collision::RayWorldHit::dist);
				if (test_dist.unwrap_or(std::numeric_limits<double>().infinity()) < current_dist.unwrap_or(std::numeric_limits<double>().infinity())) {
						closest = collision::RayWorldHit(blkinter.unwrap(), object);
				}
			}
		return closest;
	}

	template<collision::ObjectPredicate U = HitQuery>
	inline collision::RayWorldCollision raybox_cast_dynamic(geo::RayBox search_ray,ecs::Ecs& world, U query = U()) {
		ecs::View< collision::Collider,  collision::DynamicCollider,ecs::Owner> colliders(world);
		collision::RayWorldCollision closest = stn::None;
		for (auto [collider, dynamic_tag,object] : colliders) {
			if (collider.effector) {
				continue;
			}
			static_assert(std::same_as<collision::Collider&, decltype(collider)>);
			if (!query(object)) {
				continue;
			}
			geo::RayCollision blkinter = geo::intersection(search_ray,collision::global_box(object));
			stn::Option<double> test_dist = blkinter.map_member(&geo::RayHit::length);
			stn::Option<double> current_dist = closest.map_member(&collision::RayWorldHit::dist);
			if (test_dist.unwrap_or(std::numeric_limits<double>().infinity()) < current_dist.unwrap_or(std::numeric_limits<double>().infinity())) {
					closest = collision::RayWorldHit(blkinter.unwrap(), object);
			}
		}
		return closest;
	}

	template<collision::ObjectPredicate U = HitQuery>
	inline collision::RayWorldCollision ray_box_cast(geo::RayBox ray_box, ecs::Ecs& world, U query=U()) {
		collision::RayWorldCollision closest_on_grid = voxtra::grid_ray_box_cast(ray_box, world.get_resource<grid::Grid>(),query);
		collision::RayWorldCollision closest_entity = raybox_cast_dynamic(ray_box, world,query);
		return stn::min_some_on_map(closest_entity, closest_on_grid,
			[&](const collision::RayWorldHit& col) {return col.dist(); });
	}
	template<collision::ObjectPredicate U = HitQuery>
	inline collision::RayWorldCollision raycast(geo::ray nray, ecs::Ecs& world, U query=U()) {
		collision::RayWorldCollision closest_on_grid = voxtra::grid_cast(nray, world.get_resource<grid::Grid>(), query);
		collision::RayWorldCollision closest_entity = raycast_dynamic(nray, world,query);
		return stn::min_some_on_map(closest_entity, closest_on_grid,
		[&](const collision::RayWorldHit& col) {return col.dist(); });
	}

	template<collision::ObjectPredicate U = HitQuery>
	inline bool boxcast_dynamic(geo::Box blk, ecs::Ecs& world, U query) {
		ecs::View< ecs::Constrained<Collider>, DynamicCollider> colliders(world);
		for (auto [collider, dynamic_tag] : colliders) {
			if (box_intersects_aabb(blk, collider)) {
				if (query(collider)) {
					return true;
				}
			}
		}
		return false;
	}
	template<collision::ObjectPredicate U = HitQuery>
	inline bool boxcast(geo::Box box, ecs::Ecs& world, U query) {
		return voxtra::boxcast_grid(box, world.get_resource<grid::Grid>(), query) || boxcast_dynamic(box, world, query);
	}
}