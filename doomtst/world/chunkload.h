#include "chunk.h"
#include "../block/blockinit.h"
#include "noise.h"
#include "../math/interval.h"
#include "../util/fileloader.h"
#include <mutex>
#include "../util/thread_split.h"
#include "../block/stone.h"
#include "../block/air.h"
#include "../block/plank.h"
#include "../block/crystal.h"
#include "../game/time.h"
#pragma once 
namespace world {
	struct GeneratedBlock {

		blocks::block_id block_id;
		Coord pos;
		GeneratedBlock(blocks::block_id blkid, Coord location) :block_id(blkid), pos(location) {

		}
	};
	struct TerrainGenerator {
		virtual block_id generate(v3::Coord pos) const = 0;
		virtual ~TerrainGenerator() = default;
	};

	struct ChunkLoadInfo {
		chunks::ChunkLocation location;
		TerrainGenerator& generator;
		array<GeneratedBlock> chunk_ids() const {
			array<int> x_index_list;
			for (size_t i = 0; i < chunks::chunk_axis; i++) {
				x_index_list.push(i);
			}

			stn::array<GeneratedBlock> ids;
			ids.unsafe_set_length(chunks::chunk_elements);
			auto func = [&](int x) {
				int ind = chunks::chunk_axis * chunks::chunk_axis * x;
				for (int y = 0; y < chunks::chunk_axis; y++) {
					for (int z = 0; z < chunks::chunk_axis; z++) {
						Coord idpos = location.to_block_pos() + Coord(x, y, z);
						blocks::block_id neid = generator.generate(idpos);
						ids.construct_at_unchecked(ind, GeneratedBlock(neid, idpos));
						ind++;
					}
				}
				};
			thread_util::par_iter_rng(x_index_list, func, 4);
			return ids;

		}
	};

	struct CreateEmptyChunk {
		CreateEmptyChunk(ChunkLoadInfo chunk_spawn_location) :spawn_location(chunk_spawn_location) {

		}
		void apply(ecs::obj& chunk_object) {
			chunks::Chunk& newchunk = chunk_object.add_component<chunks::Chunk>(spawn_location.location);
			chunk_object.add_component<chunks::ChunkMesh>(spawn_location.location,chunk_object.world().get_resource<renderer::Renderer>());
		}
		ChunkLoadInfo spawn_location;
	};

	struct LoadFromNoise {
		ChunkLoadInfo location;
		LoadFromNoise(ChunkLoadInfo chunk_location) :location(chunk_location) {
		}

		void apply(ecs::obj& chunk_object) {

			BlockRegistry& registry = chunk_object.world().get_resource<blocks::BlockRegistry>();
			CreateEmptyChunk(location).apply(chunk_object);
			chunks::Chunk& newchunk = chunk_object.get_component<chunks::Chunk>();
			chunks::ChunkMesh& mesh = chunk_object.get_component<chunks::ChunkMesh>();


			timing::TimeProfiler profiler = timing::TimeProfiler();
			array<GeneratedBlock> statemap = location.chunk_ids();

			double value = profiler.milliseconds();
			for (GeneratedBlock& block : statemap) {
				newchunk.block_list.push(GenerateBlock{ .id = block.block_id,
						.loc = block.pos,
						.block_face= math::up2d,
						.direction = math::up_3d,
						.mesh = mesh,
						.registry = registry
					}.spawn(chunk_object.world()));
			}
			double val2= profiler.milliseconds();
			if (25<val2) {
				int l = 3;
			}
			double u = 1;
		}


	};
	struct LoadChunkFromFile {
		ChunkLoadInfo location;
		LoadChunkFromFile(ChunkLoadInfo chunk_location) :location(chunk_location) {
		}
		void apply(ecs::obj& chunk_object) {
			stn::file_handle file = stn::file_handle(chunks::file_name(location.location, chunk_object.world().get_resource<grid::World>()), stn::FileMode::ReadBinary);

			CreateEmptyChunk(location).apply(chunk_object);
			chunks::Chunk& newchunk = chunk_object.get_component<chunks::Chunk>();
			BlockRegistry& registry = chunk_object.world().get_resource<blocks::BlockRegistry>();
			for (int x = 0; x < chunks::chunk_axis; x++) {
				for (int y = 0; y < chunks::chunk_axis; y++) {
					for (int z = 0; z < chunks::chunk_axis; z++) {
						block_id block_id_for = stn::file_serializer<block_id>().read(file);
						math::Direction2d dir = stn::file_serializer<math::Direction2d>().read(file);
						math::Direction3d align = stn::file_serializer<math::Direction3d>().read(file);
						Coord pos = Coord(x, y, z) + location.location.to_block_pos();
						newchunk.block_list.push(GenerateBlock{ .id = block_id_for,
						.loc = pos,
						.block_face = dir,
						.direction = align,
						.mesh = chunk_object.get_component<chunks::ChunkMesh>(),
						.handle = file,
						.registry = registry
							} .spawn(chunk_object.world()));

					}
				}
			}
		}

	};
	struct ChunkLoaded {
		ChunkLoaded(chunks::ChunkLocation loaded_position) :pos(loaded_position) {

		}

		chunks::ChunkLocation pos;
	};
	struct CreateChunk {
		ChunkLoadInfo location;
		CreateChunk(ChunkLoadInfo chunk_location) :location(chunk_location) {

		}
		void apply(ecs::obj& object) const {
			if (fileexists(chunks::file_name(location.location, object.world().get_resource<grid::World>()))) {
				LoadChunkFromFile(location).apply(object);
			}
			else {
				LoadFromNoise(location).apply(object);
			}
			object.world().write_event(ChunkLoaded(location.location));
		}

	};


}