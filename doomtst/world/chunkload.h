#include "chunk.h"
#include "../block/blockinit.h"
#include "noise.h"
#include "../math/interval.h"
#include "../util/fileloader.h"
#include <mutex>
#include "../util/thread_split.h"
#pragma once 
namespace Chunk {
	enum biometype {
		mossybiome = 0,
		normalbiome = 1,
		open_stone = 2,

	};
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
	inline blocks::block_id generatechunkvalfromnoise(Point3 pos, math::NoiseMap& map, math::NoiseMap& crazy, math::NoiseMap& slow) {
		if (generateflat) {
			if (0 < pos.y) {
				return minecraftair;
			}
			return minecraftstone;
		}
		Point3 scaled_pos = pos * blocksize;

		double carve1 = map(scaled_pos + Vec3(0, scaled_pos.y, 0));
		double carve2 = map(scaled_pos + Coord(0, 103, 40));
		double biome = slow(scaled_pos + Coord(893, 103, 40));
		double cave_region = slow(scaled_pos + Coord(893, 103, 40));
		double random_n = crazy(scaled_pos + Coord(101, 300, 33));

		return idfromnoise(biome, random_n, carve1, carve2, cave_region);



	}
	struct idblock {

		blocks::block_id block_id;
		Coord pos;
		idblock(blocks::block_id blkid, Coord loc) {
			block_id = blkid;
			pos = loc;
		}
		idblock() {
			block_id = minecraftair;
			pos = ZeroCoord;
		}
	};

	struct idmap {
		math::NoiseMap slow;
		math::NoiseMap map;
		math::NoiseMap middle_map;
		math::NoiseMap crazy;
		array<idblock> ids;
		Coord loc;
		blocks::block_id id_at(Coord pos) {

			pos -= loc * Chunk::chunkaxis;
			if (inrange(int(pos.x), 0, Chunk::chunkaxis - 1)) {
				if (inrange(int(pos.y), 0, Chunk::chunkaxis - 1)) {

					if (inrange(int(pos.z), 0, Chunk::chunkaxis - 1)) {
						return ids[Chunk::chunkaxis * Chunk::chunkaxis * pos.x + Chunk::chunkaxis * pos.y + pos.z].block_id;
					}
				}
			}
			return minecraftair;

		}
		void iterate(int x, std::mutex& mtx) {
			int ind = Chunk::chunkaxis * Chunk::chunkaxis * x;
			for (int y = 0; y < Chunk::chunkaxis; y++) {
				for (int z = 0; z < Chunk::chunkaxis; z++) {
					Coord idpos = loc * Chunk::chunkaxis + Coord(x, y, z);

					blocks::block_id neid = generatechunkvalfromnoise(idpos, map, crazy, slow);
					mtx.lock();
					ids.reach(ind) = idblock(neid, idpos);
					mtx.unlock();
					ind++;
				}
			}

		}
		idmap(Coord location) :
			slow(math::noise_parameters(1, 1, .005, 1.2)),
			map(math::noise_parameters(3, .6, .02, 1.2)),
			crazy(math::noise_parameters(4, 1., .005, 1.2)),
			middle_map(math::noise_parameters(2, 1, .005, 1.2)) {

			array<int> x_index_list;
			for (size_t i = 0; i < Chunk::chunkaxis; i++) {
				x_index_list.push(i);
			}

			loc = location;
			ids = array<idblock>();
			std::mutex lck;
			auto func = [&lck, this](int x) {iterate(x, lck); };
			thread_util::par_iter(x_index_list.begin(), x_index_list.end(), func, 4);


		}
		

	};

	struct CreateEmptyChunk :ecs::Recipe {
		CreateEmptyChunk(ChunkLocation chunk_spawn_location) :spawn_location(chunk_spawn_location) {

		}
		void apply(ecs::obj& chunk_object) {
			Chunk::chunk& newchunk = chunk_object.add_component<Chunk::chunk>(spawn_location);
			chunk_object.add_component<Chunk::chunkmesh>();
		}
		ChunkLocation spawn_location;
	};

	struct LoadFromNoise {
		ChunkLocation location;
		LoadFromNoise(ChunkLocation chunk_location) :location(chunk_location) {
		}

		void apply(ecs::obj& chunk_object) {
			CreateEmptyChunk(location).apply(chunk_object);
			Chunk::chunk& newchunk = chunk_object.get_component<Chunk::chunk>();
			Chunk::chunkmesh& mesh = chunk_object.get_component<Chunk::chunkmesh>();
			idmap statemap = idmap(location.position);
			size_t ind = 0;
			for (int x = 0; x < Chunk::chunkaxis; x++) {
				for (int y = 0; y < Chunk::chunkaxis; y++) {
					for (int z = 0; z < Chunk::chunkaxis; z++) {
						Coord pos = statemap.ids[ind].pos;
						blocks::block_id block_id = statemap.ids[ind].block_id;
						newchunk.block_list[ind]=GenerateBlock(mesh,block_id, pos, math::Direction2d(math::up2d), math::Direction3d(math::up_3d)).spawn(chunk_object.world());
						ind++;
					}
				}
			}
		}


	};
	struct LoadChunkFromFile :ecs::Recipe {
		ChunkLocation location;
		LoadChunkFromFile(ChunkLocation chunk_location) :location(chunk_location) {
		}
		void apply(ecs::obj& chunk_object) {
			stn::file_handle file = stn::file_handle(Chunk::getcorefilename(location), stn::FileMode::ReadBinary);
			stn::array<unsigned short> bytelist = file.read<unsigned short>(Chunk::chunksize);

			file.seek(SeekOrigin::Begin, Chunk::chunksize * 2);
			stn::array<unsigned short> randomproperties = file.read<unsigned short>(Chunk::chunksize);
			CreateEmptyChunk(location).apply(chunk_object);
			Chunk::chunk& newchunk = chunk_object.get_component<Chunk::chunk>();
			size_t ind = 0;
			for (int x = 0; x < Chunk::chunkaxis; x++) {
				for (int y = 0; y < Chunk::chunkaxis; y++) {
					for (int z = 0; z < Chunk::chunkaxis; z++) {
						Coord pos = Coord(x, y, z) + location.position* Chunk::chunkaxis;
						blocks::block_id blockid = static_cast<blocks::block_id>(bytelist[ind] & unsigned char(255));
						byte dirprop = bytelist[ind] >> unsigned char(8);
						math::Direction3d mesh_attach_direction = math::Direction3d(math::DirectionIndex3d(dirprop >> unsigned char(3)));
						math::Direction2d mesh_face = math::Direction2d(math::DirectionIndex2d(dirprop & unsigned char(7)));
						newchunk.block_list[ind] = GenerateBlock(chunk_object.get_component<Chunk::chunkmesh>(), blockid, pos, math::Direction2d(math::up2d), math::Direction3d(math::up_3d)).spawn(chunk_object.world());

						ind++;
					}
				}
			}
		}

	};
	struct chunk_loaded {
		chunk_loaded(Chunk::ChunkLocation loaded_position) :pos(loaded_position) {

		}
		Chunk::ChunkLocation pos;
	};
	struct CreateChunk:ecs::Recipe {
		ChunkLocation location;
		CreateChunk(ChunkLocation chunk_location):location(chunk_location) {

		}
		void apply(ecs::obj& object) {
			if (fileexists(Chunk::getcorefilename(location))) {
				LoadChunkFromFile(location).apply(object);
			}
			else {
				LoadFromNoise(location).apply(object);
			}
			object.world().write_event(chunk_loaded(location));
		}

	};
	struct ChunkLoader :ecs::System {
		void run(ecs::Ecs& world) {


		}


	};
		

}