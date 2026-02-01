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
	inline biometype get_biome(float biome) {

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
	inline stn::Option< blocks::block_id> chaotic_overide(float chaotic, biometype biome) {
		if (math::range(0.009, .01f).contains(chaotic) && biome == normalbiome) {
			return minecraftcrystal;
		}
		if (math::range(.01f, .03f).contains(chaotic)) {
			return get_secondary_block(biome);
		}
		if (math::range(.398, .4f).contains(chaotic) && biome == normalbiome) {
			return minecraftironore;
		}
		return stn::None;
	}
	inline blocks::block_id idfromnoise(float biome, float chaotic, float cave_carve, float cave_carve2, float should_cave) {

		const float offset = .3f;
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
	inline blocks::block_id generatechunkvalfromnoise(Point3 pos, NoiseMap& map, NoiseMap& crazy, NoiseMap& slow) {
		if (generateflat) {
			if (0 < pos.y) {
				return minecraftair;
			}
			return minecraftstone;
		}
		Point3 scaled_pos = pos * blocksize;

		Coord localpos;
		localpos.x = symmetric_mod(scaled_pos.x, Chunk::chunkaxis);

		localpos.y = symmetric_mod(scaled_pos.y, Chunk::chunkaxis);

		localpos.z = symmetric_mod(scaled_pos.z, Chunk::chunkaxis);
		float carve1 = map(scaled_pos + Vec3(0, scaled_pos.y, 0));
		float carve2 = map(scaled_pos + Coord(0, 103, 40));
		float biome = slow(scaled_pos + Coord(893, 103, 40));
		float cave_region = slow(scaled_pos + Coord(893, 103, 40));
		float random_n = crazy(scaled_pos + Coord(101, 300, 33));

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
		NoiseMap slow;
		NoiseMap map;
		NoiseMap middle_map;
		NoiseMap crazy;
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
		void iterate(size_t x, std::mutex& mtx) {
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
			slow(noise_parameters(1, 1, .005, 1.2)),
			map(noise_parameters(3, .6, .02, 1.2)),
			crazy(noise_parameters(4, 1., .005, 1.2)),
			middle_map(noise_parameters(2, 1, .005, 1.2)) {

			array<size_t> x_index_list;
			for (auto i = 0; i < Chunk::chunkaxis; i++) {
				x_index_list.push(i);
			}

			loc = location;
			ids = array<idblock>();
			std::mutex lck;
			auto func = [&lck, this](size_t x) {iterate(x, lck); };
			thread_util::par_iter(x_index_list.begin(), x_index_list.end(), func, 4);


		}
		void clear() {
			map.clear();
			crazy.clear();
			slow.clear();
			middle_map.clear();
		}

	};

	struct CreateEmptyChunk :ecs::Recipe {
		CreateEmptyChunk(v3::Coord chunk_spawn_location) :spawn_location(chunk_spawn_location) {

		}
		void apply(ecs::obj& chunk_object) {
			Chunk::chunk& newchunk = chunk_object.add_component<Chunk::chunk>(spawn_location);
			chunk_object.add_component<Chunk::chunkmesh>();
			for (int i = 0; i < Chunk::chunksize; i++) {
				newchunk.block_list[i] = chunk_object.world().spawn_empty();
			}
		}
		v3::Coord spawn_location;
	};

	struct LoadFromNoise {
		v3::Coord location;
		LoadFromNoise(v3::Coord chunk_location) :location(chunk_location) {
		}

		void apply(ecs::obj& chunk_object) {
			CreateEmptyChunk(location).apply(chunk_object);
			Chunk::chunk& newchunk = chunk_object.get_component<Chunk::chunk>();
			idmap statemap = idmap(location);
			size_t ind = 0;
			for (int x = 0; x < Chunk::chunkaxis; x++) {
				for (int y = 0; y < Chunk::chunkaxis; y++) {
					for (int z = 0; z < Chunk::chunkaxis; z++) {
						Coord pos = statemap.ids[ind].pos;
						blocks::block_id block_id = statemap.ids[ind].block_id;
						GenerateBlock(chunk_object.get_component<Chunk::chunkmesh>(),block_id, pos, Dir::Dir2d(Dir::up2d), Dir::Dir3d(Dir::up3d)).apply(newchunk.block_list[ind]);
						ind++;
					}
				}
			}
			statemap.clear();
		}


	};
	struct LoadChunkFromFile :ecs::Recipe {
		v3::Coord location;
		LoadChunkFromFile(v3::Coord chunk_location) :location(chunk_location) {
		}
		void apply(ecs::obj& chunk_object) {
			stn::file_handle file = stn::file_handle(Chunk::getcorefilename(location), stn::FileMode::ReadBinary);
			stn::array<unsigned short> bytelist = file.read<unsigned short>(Chunk::chunksize);

			file.seek(SeekOrigin::Begin, Chunk::chunksize * 2);
			stn::array<unsigned short> randomproperties = file.read<unsigned short>(Chunk::chunksize);
			CreateEmptyChunk(location).apply(chunk_object);
			Chunk::chunk& newchunk = chunk_object.get_component<Chunk::chunk>();
			int i = 0;
			for (int x = 0; x < Chunk::chunkaxis; x++) {
				for (int y = 0; y < Chunk::chunkaxis; y++) {
					for (int z = 0; z < Chunk::chunkaxis; z++) {
						Coord blockpos = Coord(x, y, z) + location * Chunk::chunkaxis;

						blocks::block_id blockid = static_cast<blocks::block_id>(bytelist[i] & unsigned char(255));
						byte dirprop = bytelist[i] >> unsigned char(8);


						Dir::Dir3d mesh_attach_direction = Dir::Dir3d(dirprop >> unsigned char(3));
						Dir::Dir2d mesh_face = Dir::Dir2d(dirprop & unsigned char(7));

						GenerateBlock(chunk_object.get_component<Chunk::chunkmesh>(), blockid, blockpos, mesh_face, mesh_attach_direction).apply(newchunk.block_list[i]);
						i++;
					}
				}
			}
		}

	};
	struct chunk_loaded {
		chunk_loaded(v3::Coord loaded_position) :pos(loaded_position) {

		}
		v3::Coord pos;
	};
	struct CreateChunk:ecs::Recipe {
		v3::Coord location;
		CreateChunk(v3::Coord chunk_location):location(chunk_location) {

		}
		void apply(ecs::obj& object) {
			if (fileexists(Chunk::getcorefilename(location))) {
				LoadChunkFromFile(location).apply(object);
			}
			LoadFromNoise(location).apply(object);
			object.world().write_event(chunk_loaded(location));
		}

	};
	struct ChunkLoader :ecs::System {
		void run(ecs::Ecs& world) {


		}


	};
		

}