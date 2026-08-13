#include "grid.h"
#include "../math/ray.h"
#include "../game/aabb.h"
#include "../math/intersection.h"
#include "../math/geometry.h"

#include "../util/Option.h"
#include "../util/unique.h"

#pragma once 
namespace voxtra {

	inline bool active_region(geo::Box box, grid::Grid& world) {
		for (v3::Coord v : geo::IntBox3d(world.get_voxel(box.min()), world.get_voxel(box.max()))) {
			if (world.get_object(v).is_some()) {
				return true;
			}
		}
		return false;
	}
	
	template<collision::ObjectPredicate U>
	inline bool boxcast_grid(geo::Box box, grid::Grid& world, U pred=U()) {
		for (v3::Coord v:geo::IntBox3d(world.get_voxel(box.min()), world.get_voxel(box.max()))) {
			stn::Option<chunks::block_object&> blk= world.get_object(v);
			if (!blk) {
				continue;
			}

			stn::Option<collision::Collider&> collider=blk.unwrap().get_component_opt<collision::Collider>();
			if (collider && pred(blk.unwrap().object())) {
				if (collision::box_intersects_aabb(box, blk.unwrap().object())) {
					return true;
				}
			}
		}
		return false;
	
	}

	template<collision::ObjectPredicate U>
	inline collision::RayWorldCollision  grid_cast(geo::ray nray, grid::Grid& grid, U pred=U()) {
		if (nray.length() == 0) {
			return stn::None;
		}
		double ray_length = nray.length();
		double travel_dist = 0;
		Vec3 ray_direction = nray.dir();
		v3::Coord sgns;
		for (size_t i = 0; i < 3; i++) {
			sgns[i] = math::zero_preserving_sign(ray_direction[i]);
		}
		v3::Point3 pos = nray.point_at(0);
		v3::Coord current_voxel;
		for (size_t i = 0; i < 3; i++) {
			//this essentially shifts it into the voxel we are going to be in removing the ambiguity of the starting position of a number like 0
			current_voxel[i] =math::floor_with_infinitesimal_shift(pos[i], sgns[i] == 1);
		}
		while (travel_dist <= ray_length) {

			stn::Option<ecs::Constrained<block>&> blk = grid.get_object(current_voxel);
			if (blk) {
				stn::Option<collision::Collider&> BlockCollider = blk.unwrap().get_component_opt<collision::Collider>();
				if (BlockCollider) {
					if (pred(blk.unwrap().object())) {
						geo::RayCollision PotentialCollision = geo::intersection(collision::global_box(blk.unwrap().object()), nray);
						if (PotentialCollision) {
							return collision::RayWorldHit(PotentialCollision.unwrap(),blk.unwrap().object());
						}
					}
				}
			}
			double min_dist = std::numeric_limits<double>::infinity();
			size_t min_ind = 0;
			for (size_t i = 0; i < 3; i++) {
				//check to avoid a division by zero
				if (sgns[i] == 0) {
					continue;
				}
				// Compute the next voxel boundary along this axis
				// For positive rays, the next boundary is current_voxel + 1
				// For negative rays, the next boundary is current_voxel
				//if you think about it currvox represents the immediate voxel we are in(note the use of the shift prevents us from_being at a integer), if we are going forard we should add one else we are above currvox, so we dont do anything
				int boundry= current_voxel[i] + (sgns[i] == 1);
				double new_val = math::abs((boundry - pos[i])/ray_direction[i]);
				if (new_val < min_dist) {
					min_dist = new_val;
					min_ind = i;
				}
			}
			travel_dist += min_dist;
			pos = nray.point_at(travel_dist);
			current_voxel += Coord(sgns[min_ind], min_ind);
		}

		return stn::None;
	}

	template<collision::ObjectPredicate U >
	inline collision::RayWorldCollision grid_ray_box_cast(geo::RayBox ray_box, grid::Grid& world, U pred=U()) {
		//expand because of floating point
		geo::Box bounding_box = ray_box.bounding_box().expanded(.01f);

		collision::RayWorldCollision closest_hit;
		for (v3::Coord crd:geo::IntBox3d(world.get_voxel(bounding_box.min()),world.get_voxel(bounding_box.max()))) {
			stn::Option<chunks::block_object&> obj = world.get_object(crd);
			if (!obj) {
				continue;
			}
			stn::Option<collision::Collider&> coll_mabye= obj.unwrap().get_component_opt<collision::Collider>();
			if (!coll_mabye||!pred(obj.unwrap().object())) {
				continue;
			}
			geo::Box box =global_box(ecs::Constrained<collision::Collider>(obj.unwrap().object()));
			geo::RayCollision hit = geo::intersection(ray_box, box);
			if (!hit) {
				continue;
			}
			geo::RayHit hit_ray = hit.unwrap();
			if (closest_hit.is_none_or([hit_ray](const collision::RayWorldHit& coll){return hit_ray.length()< coll.dist();})) {
				closest_hit = collision::RayWorldHit(hit_ray, obj.unwrap().object());
			}
		}

		return closest_hit;
	}

	inline stn::Option<geo::Box> find_empty_space(v3::Scale3 scale, grid::Grid& world,size_t max_trials=40) {
		for (size_t tst = 0; tst < max_trials; tst++) {
			double ranx = (random::random() - .5) * 2;
			double rany = (random::random() - .5) * 2;
			double ranz = (random::random() - .5) * 2;
			v3::Point3 test_pos = (Vec3(ranx, rany, ranz) * (world.dim_axis) / 2 + world.grid_pos.position) * chunks::chunk_axis;
			geo::Box test_box = geo::Box(test_pos, scale);
			if (!boxcast_grid(test_box, world, collision::SolidPredicate())) {
				return stn::Option<geo::Box>(test_box);
			}
		}

		return stn::None;
	}
	inline geo::Box move_left_until_air(geo::Box box,grid::Grid& world,blocks::BlockRegistry& registry) {
		while (true) {
			v3::Coord lowest= world.get_voxel(box.min());
			v3::Coord highest= world.get_voxel(box.max()+v3::Vec3(1,1,1));
			bool only_air = true;

			for (int x = lowest.x; x <= highest.x; x++) {
				for (int y = lowest.y; y <= highest.y; y++) {
					for (int z = lowest.z; z <= highest.z; z++) {
						if (!registry.is<blocks::AirBlock>(world.generator->generate(v3::Coord(x,y,z)))) {
							only_air = false;	
						}
					}

				}
			}
			if (only_air) {
				break;
			}
			box.center.x +=2;

		}
		return box;
	}
	inline stn::Option<geo::Box> find_ground_at(geo::Box test_box, grid::Grid& world) {

		if (boxcast_grid(test_box.expanded(-1/20.f), world, collision::SolidPredicate())) {
			return stn::None;
		}
		geo::RayBox box_ray = geo::RayBox(geo::ray::from_offset(test_box.center, v3::Vec3(0, -50, 0)), test_box.scale);
		collision::RayWorldCollision col = grid_ray_box_cast(box_ray, world, collision::SolidPredicate());
		if (!col) {
			return stn::None;
		}
		geo::Box hit_box = test_box.with_center(col.unwrap().hit.ray.end);
		if (active_region(hit_box,world)) {
			return hit_box;
		}
		return stn::None;
	}
	inline stn::Option<geo::Box> find_ground(v3::Scale3 scale, grid::Grid& world,size_t max_ground_checks=200,size_t max_box_trials=200) {
		for (size_t i = 0; i < max_ground_checks; i++) {

			stn::Option<geo::Box> test_box_opt = find_empty_space(scale, world,max_box_trials);
			if (test_box_opt) {
				stn::Option<geo::Box> at = find_ground_at(test_box_opt.unwrap(), world);
				if (at.is_some()) {
					return at.unwrap();
				}
			}
		}
		return stn::None;
	}
}
