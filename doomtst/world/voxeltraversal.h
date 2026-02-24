#include "grid.h"
#include "../math/ray.h"
#include "../game/aabb.h"
#include "../math/intersection.h"
#include "../math/geometry.h"

#include "../util/Option.h"
#include "../util/unique.h"
#pragma once 
namespace voxtra {

	struct RayWorldHit
	{
		aabb::Collider& collider;
		geointersect::RayHit Hit;
		RayWorldHit(geointersect::RayHit rayHit, aabb::Collider& WorldCollider) :Hit(rayHit),collider(WorldCollider){}
		Point3 intersection() const {
			return Hit.intersectionpoint;
		}
		
		ecs::obj owner() const {
			return collider.owner();
		}
		double dist() const {
			return Hit.dist;
		}
		math::ray ray() const {
			return Hit.Ray;
		}
	};
	using WorldRayCollision = stn::Option<RayWorldHit>;
	enum class GridTraverseMode {
		countnormal = 0,
		countall = 1,
		countsolid = 2,

	};


	inline bool boxcast_grid(geo::Box Box, grid::Grid& world) {
		array<ecs::obj> blocks_in_range = world.voxel_in_range(Box);
		for (ecs::obj PotentialCollision : blocks_in_range) {
			stn::Option<aabb::Collider&> Collider = PotentialCollision.get_component_opt<aabb::Collider>();
			if (Collider && PotentialCollision.get_component<blocks::block>().solid() && !Collider.unwrap().effector) {
				if (aabb::box_intersects_aabb(Box, Collider.unwrap())) {
					return true;
				}
			}
		}
		return false;
	}
	inline bool counttablevoxel(block& blk, GridTraverseMode trav) {
		if ((trav == GridTraverseMode::countsolid || trav == GridTraverseMode::countnormal) && !blk.solid()) {
			return false;

		}
		if (blk.owner().get_component<aabb::Collider>().effector) {
			return trav != GridTraverseMode::countnormal;
		}
		return true;
	}

	inline WorldRayCollision  travvox(math::ray nray, GridTraverseMode trav,grid::Grid& grid) {
		if (nray.length() == 0) {
			return stn::None;
		}
		double ray_length = nray.length();
		double travel_dist = 0;
		Vec3 norm_ray = nray.dir();
		Vec3 conv_each;
		v3::Coord sgns;
		for (size_t i = 0; i < 3; i++) {
			//if its empty we set the direction to none
			if (norm_ray[i] == 0) {
				conv_each[i] = 0;
				sgns[i] = 0;
			}
			else {
				conv_each[i] = abs(1 / norm_ray[i]);
				sgns[i] = sign(norm_ray[i]);
			}

		}
		v3::Point3 pos = nray.start - (norm_ray * 1e-6);

		Coord curvox = grid.get_voxel(pos);
		Coord Boundry;
		for (size_t i = 0; i < 3; i++) {
			Boundry[i] = next_boundary(pos[i], sgns[i] == 1);
		}
		while (travel_dist <= ray_length) {
			WorldRayCollision Collision = stn::None;
			for (int x = extended_range(pos.x).first; x <= extended_range(pos.x).second; ++x) {
				for (int y = extended_range(pos.y).first; y <= extended_range(pos.y).second; ++y) {
					for (int z = extended_range(pos.z).first; z <= extended_range(pos.z).second; ++z) {
						stn::Option<ecs::Constrained<block>&> blk = grid.get_object(Coord(x, y, z));
							if (blk.is_none()) {
								continue;
							}
						stn::Option<aabb::Collider&> BlockCollider = blk.unwrap().get_component_opt<aabb::Collider>();
						if (!BlockCollider) {
							continue;
						}
						if (!counttablevoxel(blk.unwrap().get<block>(), trav)) {
							continue;
						}
						geointersect::boxRayCollision PotentialCollision = geointersect::intersection(BlockCollider.unwrap().global_box(), nray);
						if (PotentialCollision && PotentialCollision.unwrap().dist < ray_length && (!Collision || PotentialCollision.unwrap().dist < Collision.unwrap().dist())) {
							Collision = PotentialCollision.map([&](geointersect::RayHit hit) {return RayWorldHit(hit, BlockCollider.unwrap()); });
						}


					}
				}
			}
			if (Collision) {
				return Collision;

			}
			double min_dist = std::numeric_limits<double>::infinity();
			size_t min_ind = 0;
			for (size_t i = 0; i < 3; i++) {
				if (sgns[i] == 0) {
					continue;
				}
				double new_val = abs(Boundry[i] - pos[i]) * conv_each[i];
				if (new_val < min_dist) {
					min_dist = new_val;
					min_ind = i;

				}
			}

			pos += norm_ray * min_dist;
			travel_dist += min_dist;
			curvox += Coord(sgns[min_ind], min_ind);
			Boundry += Coord(sgns[min_ind], min_ind);

		}



		return stn::None;
	}
	inline stn::Option<geo::Box> findemptyspace(v3::Scale3 scale,grid::Grid& world) {
		size_t max_tst = 40;
		for (size_t tst = 0; tst < max_tst; tst++)
		{
			double ranx = (random::random() - .5) * 2;
			double rany = (random::random() - .5) * 2;
			double ranz = (random::random() - .5) * 2;
			v3::Point3 test_pos = (Vec3(ranx, rany, ranz) * (world.dim_axis) / 2 + world.grid_pos.position) * Chunk::chunk_axis;
			geo::Box test_box = geo::Box(test_pos,scale);
			if (!boxcast_grid(test_box,world))
			{
				return stn::Option<geo::Box>(test_box);
			}
		}

		return stn::None;
	}
	inline stn::Option<geo::Box> findground(v3::Scale3 scale,grid::Grid& world) {
		size_t max_checks = 100;
		for (size_t checks = 0; checks < max_checks; checks++)
		{


			stn::Option<geo::Box> test_box = findemptyspace(scale, world);
			if (!test_box)
			{
				continue;
			}
			geo::Box ground_tst_box = geo::Box(test_box.unwrap().center - Vec3(0, scale.y, 0), v3::Scale3(scale.x, .1f, scale.z));;
		
			if (boxcast_grid(ground_tst_box,world))
			{
				return test_box;
			}
		
		
		}
		return stn::None;
	}
}
