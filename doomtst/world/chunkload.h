#include "chunk.h"
#include "../block/blockinit.h"
#include "noise.h"
#include "../math/interval.h"
#include "../util/fileloader.h"
#include <mutex>
#include "../util/thread_split.h"
#include "../block/stone.h"
#include "../block/air.h"
#pragma once 
namespace Chunks {
	enum biometype {
		mossybiome = 0,
		normalbiome = 1,
		open_stone = 2,

	};
	/*
	inline biometype get_biome(double biome) {

		if (math::range(0, 1).contains(biome)) {
			return normalbiome;
		}
		if (math::range(-1, -.5).contains(biome)) {
			return normalbiome;
		}
		return mossybiome;

	}
	inline blocks::block_id get_default_block(biometype biome) {

		if (biome == normalbiome) {
			return minecraftstone;
		}
		if (biome == mossybiome) {
			return minecrafttreestone;
		}

	}
	inline blocks::block_id get_secondary_block(biometype biome) {

		if (biome == normalbiome) {
			return minecraftstone;
		}
		if (biome == mossybiome) {
			return minecraftmoss;
		}
		return minecrafttreestone;
	}
	inline stn::Option< blocks::block_id> chaotic_overide(double chaotic, biometype biome) {
		if (math::range(0.009, .01f).contains(chaotic) && biome == normalbiome) {
			return minecraftstone;
		}
		if (math::range(.01f, .03f).contains(chaotic)) {
			return get_secondary_block(biome);
		}
		if (math::range(.398, .4f).contains(chaotic) && biome == normalbiome) {
			return minecraftstone;
		}
		return stn::None;
	}
	inline blocks::block_id idfromnoise(double biome, double chaotic, double cave_carve, double cave_carve2, double should_cave) {

		const double offset = .3f;
		biometype biome_type = get_biome(biome);
		blocks::block_id main_block = get_default_block(biome_type);

		stn::Option< blocks::block_id> overload = chaotic_overide(chaotic, biome_type);
		if (overload) {
			main_block = overload.unwrap();
		}
		blocks::block_id neid = main_block;

		if (inrange(cave_carve, -offset, offset)) {
			if (inrange(cave_carve2, -offset, offset)) {
				if (0 < should_cave) {
					neid = minecraftair;
				}
			}


		}
		if (.99 < should_cave) {
			neid = minecraftair;
		}
		return neid;
	}

	*/

	//catched noise_maps for a world
	struct NoiseMaps :ecs::resource {
		stn::type_map < math::NoiseMap> maps;

		template<typename T>
		const math::NoiseMap& register_map(const math::NoiseParameters& params) {
			if (!maps.contains<T>()) {
				maps.set<T>(math::NoiseMap(params));
			}
			return maps.at<T>().unwrap();
		}
	};
	inline blocks::block_id generatechunkvalfromnoise(Point3 pos,const math::NoiseMap& map, const math::NoiseMap& crazy, const math::NoiseMap& slow, const BlockRegistry& registry) {
		if (generateflat) {
			if (0 < pos.y) {
				return registry.get_id<blocks::AirBlock>();
			}
			return registry.get_id<blocks::StoneBlock>();
		}
		Point3 scaled_pos = pos * blocksize;

		double carve1 = map(scaled_pos + Vec3(0, scaled_pos.y, 0));
		double carve2 = map(scaled_pos + Coord(0, 103, 40));
		double biome = slow(scaled_pos + Coord(893, 103, 40));
		double cave_region = slow(scaled_pos + Coord(893, 103, 40));
		double random_n = crazy(scaled_pos + Coord(101, 300, 33));
		stn::throw_logic_error("need to make a better generation system");
		//return idfromnoise(biome, random_n, carve1, carve2, cave_region);



	}
	struct idblock {

		blocks::block_id block_id;
		Coord pos;
		idblock(blocks::block_id blkid, Coord loc) :block_id(blkid) {
			pos = loc;
		}
		idblock(blocks::block_id id) : block_id(id) {
			pos = ZeroCoord;
		}
	};

	struct idmap {
		const math::NoiseMap& slow;
		const math::NoiseMap& map;
		const math::NoiseMap& middle_map;
		const math::NoiseMap& crazy;
		array<idblock> ids;
		Coord loc;
		const BlockRegistry& registry;
		blocks::block_id id_at(Coord pos) {

			pos -= loc * Chunks::chunk_axis;
			if (in_range(int(pos.x), 0, Chunks::chunk_axis - 1)) {
				if (in_range(int(pos.y), 0, Chunks::chunk_axis - 1)) {

					if (in_range(int(pos.z), 0, Chunks::chunk_axis - 1)) {
						return ids[Chunks::chunk_axis * Chunks::chunk_axis * pos.x + Chunks::chunk_axis * pos.y + pos.z].block_id;
					}
				}
			}
			return registry.get_id<blocks::AirBlock>();

		}
		void iterate(int x, std::mutex& mtx) {
			int ind = Chunks::chunk_axis * Chunks::chunk_axis * x;
			for (int y = 0; y < Chunks::chunk_axis; y++) {
				for (int z = 0; z < Chunks::chunk_axis; z++) {
					Coord idpos = loc * Chunks::chunk_axis + Coord(x, y, z);

					blocks::block_id neid = generatechunkvalfromnoise(idpos, map, crazy, slow, registry);
					mtx.lock();
					ids[ind] = idblock(neid, idpos);
					mtx.unlock();
					ind++;
				}
			}

		}

		struct slow_map {
		};
		struct normal_noise_map {
		};
		struct crazy_noise_map {
		};
		struct middle_map {
		};
		idmap(Coord location, const BlockRegistry& block_registry, NoiseMaps& maps) :

			slow(maps.register_map<slow_map>(math::NoiseParameters(1, 1, .005, 1.2))),
			map(maps.register_map<slow_map>(math::NoiseParameters(3, .6, .02, 1.2))),
			crazy(maps.register_map<slow_map>(math::NoiseParameters(4, 1., .005, 1.2))),
			middle_map(maps.register_map<slow_map>(math::NoiseParameters(2, 1, .005, 1.2))), registry(block_registry) {

			array<int> x_index_list;
			for (size_t i = 0; i < Chunks::chunk_axis; i++) {
				x_index_list.push(i);
			}

			loc = location;
			ids = array<idblock>(4096, idblock(registry.get_id<AirBlock>()));
			std::mutex lck;
			auto func = [&lck, this](int x) {iterate(x, lck); };
			thread_util::par_iter(x_index_list.begin(), x_index_list.end(), func, 4);


		}


	};
	struct ChunkLoaderPlugin :ecs::System {
		void build(Core::App& world) {


		}


	};
	struct CreateEmptyChunk {
		CreateEmptyChunk(ChunkLocation chunk_spawn_location) :spawn_location(chunk_spawn_location) {

		}
		void apply(ecs::obj& chunk_object) {
			Chunks::chunk& newchunk = chunk_object.add_component<Chunks::chunk>(spawn_location);
			chunk_object.add_component<Chunks::chunkmesh>(spawn_location);
		}
		ChunkLocation spawn_location;
	};

	struct LoadFromNoise {
		ChunkLocation location;
		LoadFromNoise(ChunkLocation chunk_location) :location(chunk_location) {
		}

		void apply(ecs::obj& chunk_object) {
			CreateEmptyChunk(location).apply(chunk_object);
			Chunks::chunk& newchunk = chunk_object.get_component<Chunks::chunk>();
			Chunks::chunkmesh& mesh = chunk_object.get_component<Chunks::chunkmesh>();
			idmap statemap = idmap(location.position, chunk_object.world().get_resource<BlockRegistry>(), chunk_object.world().insert_resource<NoiseMaps>());
			size_t ind = 0;
			for (int x = 0; x < Chunks::chunk_axis; x++) {
				for (int y = 0; y < Chunks::chunk_axis; y++) {
					for (int z = 0; z < Chunks::chunk_axis; z++) {
						Coord pos = statemap.ids[ind].pos;
						blocks::block_id block_id = statemap.ids[ind].block_id;
						newchunk.block_list.push(GenerateBlock{ .id = block_id,
						.loc = pos,
						.face = math::up2d,
						.direction = math::up_3d,
						.mesh = mesh }.spawn(chunk_object.world()));
						ind++;
					}
				}
			}
		}


	};
	struct LoadChunkFromFile {
		ChunkLocation location;
		LoadChunkFromFile(ChunkLocation chunk_location) :location(chunk_location) {
		}
		void apply(ecs::obj& chunk_object) {
			stn::file_handle file = stn::file_handle(Chunks::file_name(location, chunk_object.world().get_resource<grid::world>()), stn::FileMode::ReadBinary);

			CreateEmptyChunk(location).apply(chunk_object);
			Chunks::chunk& newchunk = chunk_object.get_component<Chunks::chunk>();
			for (int x = 0; x < Chunks::chunk_axis; x++) {
				for (int y = 0; y < Chunks::chunk_axis; y++) {
					for (int z = 0; z < Chunks::chunk_axis; z++) {
						block_id block_id_for = stn::file_serializer<block_id>().read(file);
						math::Direction2d dir = stn::file_serializer<math::Direction2d>().read(file);
						math::Direction3d align = stn::file_serializer<math::Direction3d>().read(file);
						Coord pos = Coord(x, y, z) + location.position * Chunks::chunk_axis;
						newchunk.block_list.push(GenerateBlock{ .id = block_id_for,
						.loc = pos,
						.face = dir,
						.direction = align,
						.mesh = chunk_object.get_component<Chunks::chunkmesh>(),
						.handle = file } .spawn(chunk_object.world()));

					}
				}
			}
		}

	};
	struct chunk_loaded {
		chunk_loaded(Chunks::ChunkLocation loaded_position) :pos(loaded_position) {

		}

		Chunks::ChunkLocation pos;
	};
	struct CreateChunk {
		ChunkLocation location;
		CreateChunk(ChunkLocation chunk_location) :location(chunk_location) {

		}
		void apply(ecs::obj& object) const {
			if (fileexists(Chunks::file_name(location, object.world().get_resource<grid::world>()))) {
				LoadChunkFromFile(location).apply(object);
			}
			else {
				LoadFromNoise(location).apply(object);
			}
			object.world().write_event(chunk_loaded(location));
		}

	};


}