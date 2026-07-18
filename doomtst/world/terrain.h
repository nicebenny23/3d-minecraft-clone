#include "WorldCoverer.h"
#include "../util/tuple.h"
#include "chunkload.h"
#pragma once
namespace world {

	enum biometype {
		normalbiome,
		redland

	};

	enum class BorderState {
		solid,
		on,
		empty
	};
	
	struct DefaultTerrainGenerator :world::TerrainGenerator {
	
		inline biometype get_biome(double biome) const {
			double min = -1;
			if (math::bounds(-1, -.97).contains(biome)) {
				return redland;
			}
			return normalbiome;

		}
		inline blocks::block_id get_redland(BorderState state) const {

				if (state == BorderState::on) {
					return registry.get_id<blocks::SoilBlock>();

				}
				return registry.get_id<blocks::SoilBlock>();

		}
		inline blocks::block_id get_stone(double chaotic, BorderState dist) const {
			double per_pick = 5;
			double per_use = per_pick / (5 * 256) / 2;

			if (dist == BorderState::solid) {
				per_use = 0;
			}

			if (math::bounds(-per_use, per_use).contains(chaotic)) {
				return registry.get_id<blocks::CrystalBlock>();
			}
			return registry.get_id<blocks::StoneBlock>();


		}
		

		inline blocks::block_id non_cave_id(double biome, double chaotic, BorderState dist) const {

			biometype biome_type = get_biome(biome);
			if (biome_type==biometype::redland) {
				return get_redland(dist);
			}
			return get_stone(chaotic, dist);
		}	math::NoiseMap normal;
		math::NoiseMap crazy;
		math::NoiseMap axis;
		math::WorleyNoiseMap water_map;
		math::NoiseMap smooth;
		BlockRegistry& registry;
		DefaultTerrainGenerator(BlockRegistry& blk_registry) :
			water_map(math::WorleyNoise{ .hemisphere = true },2000),
			smooth(math::OctaveSeries{ .starting_period = 1.0f }, 200000),
			normal(math::OctaveSeries{ .octaves = 3,.starting_period = 1,.period_factor = .5f,.amplification_factor = .5f }, 200000),
			crazy(math::OctaveSeries{ .starting_period = 3 }, 20000),
			axis(math::OctaveSeries{ .starting_period = 90 }, 20000),
			registry(blk_registry) {

		}
		v3::Vec3 warp_position(v3::Vec3 pos,size_t seed) const {
			double speed_scale = 10;
			v3::Vec3 warp(axis(pos, 3* seed), axis(pos, 3* seed+1), axis(pos, 3 * seed + 2));
			return pos + warp * speed_scale;
		}
		math::bounds caveness(v3::Point3 pos) const {
			double world_caveness = normal(pos / 60.0, 9);
			world_caveness = (world_caveness + 1) / 2;
			world_caveness = math::ease_in_out_power(math::ease_in_power(world_caveness, 2.5), 2.5);
			double big_carver_bound_size = math::bounds(.09, .175).lerp(world_caveness);
			
			math::bounds big_carver_bounds = math::bounds::from_center_radius(0, big_carver_bound_size);
			return big_carver_bounds;
		}
		double global_dist_one(v3::Vec3 pos, math::bounds bounds) const {

			double bit_carver_size = 100;
			pos = warp_position(pos, 6);
			double big_carver_1 = normal(pos / bit_carver_size, 4);
			return bounds.signed_distance_to(big_carver_1) * bit_carver_size;

		}
		double global_dist_two(v3::Vec3 pos, math::bounds bounds) const {

			double bit_carver_size = 100;
			pos = warp_position(pos,5);
			double big_carver_2 = normal(pos / bit_carver_size, 5);
			return bounds.signed_distance_to(big_carver_2) * bit_carver_size;

		}
		bool water(v3::Vec3 pos) const {

			double bit_carver_size = 80;
			math::bounds water_bounds = math::bounds(0,.1);
			double flt = water_map(pos / bit_carver_size, 15);
				return water_bounds.contains(flt);
		}
		BorderState get_state(v3::Coord position) const {
		
			v3::Vec3 pos = position;
			
			double noise = 0;
			math::bounds big_carver_bounds = caveness(pos);
			double global_dist_1 = global_dist_one(pos, big_carver_bounds);
			double signed_distance = global_dist_1;
			double global_dist_2 = 0;
			if (signed_distance < 12) {
				global_dist_2 = global_dist_two(pos, big_carver_bounds);
			}
			stn::set_max(signed_distance, global_dist_2);
			if (math::bounds(0, 12).contains_exact(signed_distance)) {
				for (math::Direction3d dir : math::Directions3d) {
					v3::Vec3 neighbor_pos = position + dir.vec();
					math::bounds big_carver_bounds = caveness(neighbor_pos);
					signed_distance = 0;
					//would rarely fail if global_dist_1 is negitive but switches to positive thats why we limit based on a second double
					double min_check_offset = -5;
					if (global_dist_1 > min_check_offset) {
						stn::set_max(signed_distance, global_dist_one(neighbor_pos, big_carver_bounds));
					}
					if (global_dist_2 > min_check_offset) {
						stn::set_max(signed_distance, global_dist_two(neighbor_pos, big_carver_bounds));
					}
					if (signed_distance == 0) {
						return BorderState::on;
					}
				}
			}

			if (signed_distance < 0) {
				return BorderState::empty;
			}
			return BorderState::solid;
		}

		block_id generate(v3::Coord position) const {
			BorderState state = get_state(position);
			v3::Vec3 pos = position;
			if (state == BorderState::empty) {
				if (water(position)) {
					return registry.get_id<WaterBlock>();
				}

				for (math::Direction3d dir : math::Directions3d) {
					if (dir != math::down_3d) {

						if (water(position + dir.coord())) {
							return registry.get_id<blocks::StoneBlock>();
						}
					}
				}
				return registry.get_id<blocks::AirBlock>();
			}
			
	
			double biome = smooth(pos/ 20, 10);
			double random_n = crazy(position, 2);

			return non_cave_id(biome, random_n, state);
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