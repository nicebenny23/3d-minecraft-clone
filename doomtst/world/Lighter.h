#pragma once
#include "grid.h"

#include "../block/block.h"
#include "../game/ecs/ecs.h"
#include "../game/ecs/message.h"
#include "../game/ecs/system.h"
#include "../math/cube_vertex.h"
#include "../math/dir.h"
#include "../math/mathutil.h"
#include "../math/vector3.h"
#include "../util/Option.h"
#include "../util/queue.h"
#include "../util/reference.h"
#include "chunk.h"
#include "chunk_mesh.h"
#include "chunkload.h"
#include <cstdint>
namespace grid {
	struct partial_darken_command {
		v3::Coord location;
		size_t old_light;
	};
	struct lighten_block_command {
		v3::Coord location;
	};
	struct shade_at {
		shade_at(blocks::block& block_for, size_t light_value) :block(block_for), light(light_value) {
		}
		stn::non_null<blocks::block> block;
		size_t light;
	};
	inline void uncatche_light(v3::Coord pos, Grid& grid) {
		Chunks::ChunkLocation min = grid.chunk_from_block_pos(pos - v3::Coord(1, 1, 1));
		Chunks::ChunkLocation max = grid.chunk_from_block_pos(pos + v3::Coord(1, 1, 1));
		for (int cx = min.position.x; cx <= max.position.x; cx++) {
			for (int cy = min.position.y; cy <= max.position.y; cy++) {
				for (int cz = min.position.z; cz <= max.position.z; cz++) {
					grid.get_chunk_object(Chunks::ChunkLocation(v3::Coord(cx, cy, cz)))
						.then([](grid::ChunkObject& object) {
						object.get_component < Chunks::chunkmesh>().mark_dirty();
							});

				}
			}
		}
	}

	struct LightRemover :ecs::System {
		LightRemover(ecs::Ecs& world) {

		}
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			stn::queue<shade_at> darkening_queue;
			for (partial_darken_command& cmd : world.read_commands<partial_darken_command>()) {
				grid
					.get_block(cmd.location)
					.then([&](blocks::block& blk) {
					if (blk.light_passing_through < cmd.old_light) {
						darkening_queue.push(shade_at(blk, cmd.old_light));
					}
						});

			}
			while (!darkening_queue.empty()) {
				shade_at shade = darkening_queue.pop();
				uncatche_light(shade.block->pos, grid);
				for (auto dir : math::Directions3d) {
					Coord look_position = shade.block->pos + dir.coord();
					grid.get_block(look_position).then([&](block& next_block) {
						if (next_block.mesh.is_transparent()) {
							if (shade.light <= next_block.light_passing_through) {
								world.write_command(lighten_block_command({ next_block.pos }));
							}
							else {
								if (shade.light) {

									darkening_queue.push(shade_at(next_block, next_block.light_passing_through));
									next_block.light_passing_through = 0;

								}
							}
						}


						});
				}
			}
		}
	};

	struct lighten_block_command_accept {
		v3::Coord location;
	};
	struct GridLighter :ecs::System {
		ecs::EventReader<Chunks::chunk_loaded> chunk_loads;
		GridLighter(ecs::Ecs& world) :chunk_loads(world.make_reader<Chunks::chunk_loaded>()) {
		}

		void push_to_queue(Chunks::block_object& blk, stn::queue<stn::non_null<block>>& lighten_queue) {
			block& block_value = blk.get_unchecked<block>();
			blk.get_component_opt<block_emmision>().then([&](block_emmision& emit) {
				stn::set_max(block_value.light_passing_through, emit.emmision);
			});
			if (block_value.light_passing_through != 0) {
				lighten_queue.emplace(block_value);
			}
		}
		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			stn::queue<stn::non_null<block>> lightening_queue;
			for (Chunks::chunk_loaded& load : chunk_loads.read()) {

				Chunks::chunk& chnk = grid.get_chunk(load.pos).expect("chunk should exist");
				for (Chunks::block_object& block : chnk) {
					push_to_queue(block, lightening_queue);

				}
				for (math::Direction3d dir : math::Directions3d) {
					grid.get_chunk(Chunks::ChunkLocation(load.pos.position + dir.coord()))
						.then([&](Chunks::chunk& cnk) {
						for (Chunks::block_object& object : cnk.on_face(-dir)) {
							push_to_queue(object, lightening_queue);
						}});
				}
			}

			for (lighten_block_command& cmd : world.read_commands<lighten_block_command>()) {
				grid.get_object(cmd.location)
					.then([&](Chunks::block_object& next_block) {push_to_queue(next_block, lightening_queue); }
					);
			}
			while (!lightening_queue.empty()) {
				block& blk = *lightening_queue.pop();
				std::uint16_t light_value = blk.light_passing_through;
				//stop queue at zero 
				if (light_value == 0) {
					continue;
				}
				uncatche_light(blk.pos, grid);

				for (auto dir : math::Directions3d) {

					Coord look_position = blk.pos + dir.coord();
					grid.get_block(look_position).then([&](blocks::block& next_block) {
						if (next_block.mesh.is_transparent()) {
							if (next_block.light_passing_through < light_value - 1) {
								next_block.light_passing_through = light_value - 1;
								lightening_queue.emplace(next_block);
							}
						}
					});
				}
			}
		}
	};
	template<WorldAccessor Grid>
	inline float get_vertex_light(v3::Coord pos, Grid& accesor) {
		std::uint16_t max = 0;
		for (math::cube_index index : math::cube_indices) {
			v3::Coord next_look_pos = pos + index.vertex();
			stn::Option<block&> blk_mabye_look = accesor.get_block(next_look_pos);
			if (blk_mabye_look) {
				block& blk(blk_mabye_look.unwrap());
				if (blk.mesh.is_transparent()) {
					stn::set_max(max, blk.light_passing_through);
				}

			}
		}
		return max / 16.0f;
	}


}
