#include "../game/camera.h"
#include "chunkload.h"
#include "../math/vector3.h"
#include "../math/geometry.h"
#pragma once
namespace grid {


	struct Grid :ecs::resource {
		//total length in one dimention

	//how many chunks it spans from the center chunk 
		const size_t rad;
		int dim_axis;
		size_t totalChunks;
		Chunk::ChunkLocation grid_pos;
		stn::array<Chunk::chunk*> chunklist;

		Grid(size_t axis);
		void load(ecs::Ecs& world);
		void updateborders();
		void destroy();


		Chunk::chunk*& operator[](int index) {
			return chunklist[index];
		}

		Point3 toBlockPos(Point3 point);
		Coord chunkfromblockpos(Coord pos);
		Coord getVoxel(Point3 pos);
		Coord get_chunk(Point3 pos);
		bool chunk_loaded(Coord loc);

		bool contains_chunk_location(Chunk::ChunkLocation loc) const;
		bool contains_chunk_location(Coord loc) const;
		int localChunkIndex(Coord NormedChunk);
		int chunkIndex(Chunk::ChunkLocation pos);
		int chunkIndex(Coord Chunk);
		Chunk::chunk* GetChunk(Coord pos);

		stn::Option<Chunk::chunk&> get_chunk(Chunk::ChunkLocation pos);
		stn::Option<Chunk::chunk&> get_chunk(Coord pos);
		Option< ecs::obj&> get_object(const v3::Coord pos);
		Option<block&> get_block(const v3::Coord pos);
		block* getBlock(const v3::Coord pos);
		ecs::obj* getObject(const v3::Coord pos);
		array<block*> voxelinrange(geo::Box span);

		size_t chunks_loaded() {
			return chunklist.pipe().count([](Chunk::chunk* ptr) {return ptr != nullptr; });
		}


		//normed pos is when pos is in the range [0...2*CtxName::cxt.Grid->length+1) for each direction;


	};


};

