#include "WorldCoverer.h"
#include "../util/tuple.h"
#include "chunkload.h"
#pragma once
namespace world {

	enum biometype {
		mossybiome = 0,
		normalbiome = 1,
		open_stone = 2,

	};
	inline biometype get_biome(double biome) {

		if (math::bounds(0, 1).contains(biome)) {
			return normalbiome;
		}
		if (math::bounds(-1, -.5).contains(biome)) {
			return normalbiome;
		}
		return mossybiome;

	}
	inline blocks::block_id get_default_block(biometype biome, const BlockRegistry& registry) {

		if (biome == normalbiome) {
			return registry.get_id<blocks::StoneBlock>();
		}
		if (biome == mossybiome) {
			return registry.get_id<blocks::StoneBlock>();
		}

	}
	inline blocks::block_id get_secondary_block(biometype biome, const BlockRegistry& registry) {
		return registry.get_id<blocks::CrystalBlock>();
	}
	inline stn::Option< blocks::block_id> chaotic_overide(double chaotic, double dist, biometype biome, const BlockRegistry& registry) {
		double per_pick = 5;
		double per_use = per_pick /(5*256)/2;

		if (dist != 0) {
			per_use=0;
		}

		if (math::bounds(-per_use, per_use).contains(chaotic)) {
			return get_secondary_block(biome, registry);
		}
		return stn::None;
	}

	inline blocks::block_id non_cave_id(double biome, double chaotic, double dist, const BlockRegistry& registry) {

		biometype biome_type = get_biome(biome);
		blocks::block_id main_block = get_default_block(biome_type, registry);

		stn::Option< blocks::block_id> overload = chaotic_overide(chaotic, dist, biome_type, registry);
		if (overload) {
			main_block = overload.unwrap();
		}
		blocks::block_id neid = main_block;

		return neid;
	}
	inline math::bounds caveness(Point3 pos, const math::NoiseMap& map) {
		double caveness = map(pos / 60.0, 9);
		caveness = (caveness + 1) / 2;
		caveness = math::ease_in_out_power(math::ease_in_power(caveness, 2.5), 3);
		double big_carver_bound_size = math::bounds(.08, .16).lerp(caveness);

		if (caveness <.000005f) {
			big_carver_bound_size=0;
		}
	
		math::bounds big_carver_bounds = math::bounds::from_center_radius(0, big_carver_bound_size);
		return big_carver_bounds;
	}

	inline blocks::block_id generatechunkvalfromnoise(Point3 pos, const math::NoiseMap& map, const math::NoiseMap& crazy, const math::NoiseMap& slow, const math::NoiseMap& axis, const BlockRegistry& registry) {
		double speed_scale = 10;
		v3::Vec3 warp(axis(pos, 10), axis(pos, 11), axis(pos, 12));

		pos += warp * speed_scale;
		Point3 scaled_pos = pos * blocksize;
		double noise = 0;
		double bit_carver_size = 100;
		math::bounds big_carver_bounds = caveness(pos, map);
		double big_carver_1 = map(pos / bit_carver_size, 4);
		double global_distance = big_carver_bounds.signed_distance_to(big_carver_1) * bit_carver_size;
		double signed_distance = global_distance;
		double big_carver_2 = map(pos / bit_carver_size, 5);
		double global_dist_2 = big_carver_bounds.signed_distance_to(big_carver_2) * bit_carver_size;
		stn::set_max(signed_distance, global_dist_2);
		if (math::bounds(0,16).contains(signed_distance)) {
			for (math::Direction3d dir : math::Directions3d) {
				math::bounds big_carver_bounds = caveness(pos+dir.vec(), map);
				if (global_distance > 0) {
					if (big_carver_bounds.contains(map((dir.vec() + pos) / bit_carver_size, 4))) {
						signed_distance = 0;
						break;
					}
				}
				if (global_dist_2 > 0) {
					if (big_carver_bounds.contains(map((dir.vec() + pos) / bit_carver_size, 5))) {
						signed_distance = 0;
						break;
					}
				}

			}
		}
		if (signed_distance < 0) {

			return registry.get_id<blocks::AirBlock>();
		}
		double biome = slow(scaled_pos, 1);
		double random_n = crazy(pos, 2);

		return non_cave_id(biome, random_n, signed_distance, registry);



	}

	struct DefaultTerrainGenerator :world::TerrainGenerator {
		NoiseMap slow;
		NoiseMap normal;
		NoiseMap crazy;
		NoiseMap axis;
		BlockRegistry& registry;
		DefaultTerrainGenerator(BlockRegistry& blk_registry)
			:slow(math::OctaveSeries{ .octaves = 2,.starting_period = 200,.period_factor = 1 / 2.f,.starting_amplifcation = 4,.amplification_factor = .5f }, 2000),
			normal(math::OctaveSeries{ .octaves = 3,.starting_period = 1,.period_factor = .5f,.starting_amplifcation = 1,.amplification_factor = .5f }, 200000),
			crazy(math::OctaveSeries{ .starting_period = 3 }, 20000),
			axis(math::OctaveSeries{ .starting_period = 90}, 20000),
			registry(blk_registry) {

		}
		block_id generate(v3::Coord pos) const {
			return generatechunkvalfromnoise(pos, normal, crazy, slow, axis, registry);
		}
	};

	struct FlatTerrainGenerator :world::TerrainGenerator {
		block_id generate(v3::Coord pos) const {
			if (-10 < pos.y) {
				return registry.get_id<AirBlock>();
			}

			return registry.get_id<StoneBlock>();
		}
		BlockRegistry& registry;
		FlatTerrainGenerator(BlockRegistry& blk_registry) :registry(blk_registry) {

		}
	};
}