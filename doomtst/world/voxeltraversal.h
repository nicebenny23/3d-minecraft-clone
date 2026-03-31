#include "grid.h"
#include "../math/ray.h"
#include "../game/aabb.h"
#include "../math/intersection.h"
#include "../math/geometry.h"

#include "../util/Option.h"
#include "../util/unique.h"

#pragma once 
namespace voxtra {

	struct RayWorldHit {
		ecs::Constrained<aabb::Collider> collider;
		geo::RayHit hit;

		RayWorldHit(geo::RayHit rayHit, ecs::Constrained<aabb::Collider> WorldCollider) :hit(rayHit), collider(WorldCollider) {
		}
		Point3 intersection() const {
			return ray().end;
		}

		ecs::obj owner() const {
			return collider.object();
		}

		double dist() const {
			return ray().length();
		}
		stn::Option<math::Direction3d> hit_direction() const {
			return hit.hit_normal;
		}

		geo::ray ray() const {
			return hit.ray;
		}
	};
	
	using RayWorldCollision = stn::Option<RayWorldHit>;
	enum class GridTraverseMode {
		countnormal = 0,
		countall = 1,
		countsolid = 2,

	};


	inline bool boxcast_grid(geo::Box Box, grid::Grid& world) {
		array<Chunks::block_object> blocks_in_range = world.voxel_in_range(Box);
		for (Chunks::block_object& PotentialCollision : blocks_in_range) {

			stn::Option<aabb::Collider&> Collider = PotentialCollision.get_component_opt<aabb::Collider>();
			if (Collider && PotentialCollision.get_component<blocks::block>().solid() && !Collider.unwrap().effector) {
				geo::Box coll_box = Collider.unwrap().global_box();
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

	inline RayWorldCollision  grid_cast(geo::ray nray, GridTraverseMode trav, grid::Grid& grid) {
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
			}
			else {
				conv_each[i] = abs(1 / norm_ray[i]);
			}
			sgns[i] = zero_sign(norm_ray[i]);
		}
		v3::Point3 pos = nray.point_at(0);
		Coord curvox = grid.get_voxel(pos);
		Coord Boundry;
		for (size_t i = 0; i < 3; i++) {
			Boundry[i] = next_boundary(pos[i], sgns[i] == 1);
		}
		while (travel_dist <= ray_length) {

			stn::Option<ecs::Constrained<block>&> blk = grid.get_object(curvox);
			if (blk) {
				stn::Option<aabb::Collider&> BlockCollider = blk.unwrap().get_component_opt<aabb::Collider>();
				if (BlockCollider) {
					if (counttablevoxel(blk.unwrap().get<block>(), trav)) {
						geo::RayCollision PotentialCollision = geo::intersection(BlockCollider.unwrap().global_box(), nray);
						if (PotentialCollision) {
							return RayWorldHit(PotentialCollision.unwrap(), ecs::Constrained<aabb::Collider>(blk.unwrap().object()));
						}
					}
				}
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
			travel_dist += min_dist;
			pos = nray.point_at(travel_dist);
			curvox += Coord(sgns[min_ind], min_ind);
			Boundry += Coord(sgns[min_ind], min_ind);
		}

		return stn::None;
	}
	inline RayWorldCollision grid_cast(geo::RayBox ray_box, grid::Grid& world) {
		//expand because of floating point
		geo::Box bounding_box = ray_box.bounding_box().expanded(.01f);

		stn::array<Chunks::block_object> boxes = world.voxel_in_range(bounding_box);
		RayWorldCollision closest_hit;
		for (Chunks::block_object& obj : boxes) {
			stn::Option<aabb::Collider&> coll_mabye= obj.get_component_opt<aabb::Collider>();
			if (!coll_mabye) {
				continue;
			}
			geo::Box box = coll_mabye.unwrap().global_box();
			geo::RayCollision hit = geo::intersection(ray_box, box);
			if (!hit) {
				continue;
			}
			geo::RayHit hit_ray = hit.unwrap();
			if (closest_hit.is_none_or([hit_ray](const voxtra::RayWorldHit& coll){return hit_ray.length()< coll.dist();})) {
				closest_hit = RayWorldHit(hit_ray, obj.object());
			}
		}

		return closest_hit;
	}
	inline stn::Option<geo::Box> findemptyspace(v3::Scale3 scale, grid::Grid& world) {
		size_t max_tst = 40;
		for (size_t tst = 0; tst < max_tst; tst++) {
			double ranx = (random::random() - .5) * 2;
			double rany = (random::random() - .5) * 2;
			double ranz = (random::random() - .5) * 2;
			v3::Point3 test_pos = (Vec3(ranx, rany, ranz) * (world.dim_axis) / 2 + world.grid_pos.position) * Chunks::chunk_axis;
			geo::Box test_box = geo::Box(test_pos, scale);
			if (!boxcast_grid(test_box, world)) {
				return stn::Option<geo::Box>(test_box);
			}
		}

		return stn::None;
	}
	inline stn::Option<geo::Box> findground(v3::Scale3 scale, grid::Grid& world) {
		size_t max_checks = 10;
		for (size_t i = 0; i < max_checks; i++) {

			stn::Option<geo::Box> test_box_opt = findemptyspace(scale, world);
			if (!test_box_opt) {
				continue;
			}
			geo::Box test_box = test_box_opt.unwrap();
			geo::RayBox box_ray = geo::RayBox(geo::ray::from_offset(test_box.center, v3::Vec3(0, -100, 0)), scale);
			RayWorldCollision col = grid_cast(box_ray, world);
			if (!col) {
				continue;
			}
			geo::Box hit_box = test_box.with_center(col.unwrap().hit.ray.end);
			bool all_loaded = true;
			if (boxcast_grid(hit_box.expanded(-1/2.0f), world)) {
				int l = 3;
			}
			for ( cube_index index: cube_indices) {
				int l = 3;
				if (!world.get_chunk(world.get_voxel(hit_box.point_at_vertex(index)))) {
					all_loaded = false;
				}
			}
			if (all_loaded) {
				return hit_box;
			}
		}
		return stn::None;
	}
}
