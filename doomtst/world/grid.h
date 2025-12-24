#include "../game/camera.h"
#include "chunkload.h"
#include "../math/vector3.h"
#include "../math/geometry.h"
#pragma once
namespace CtxName {
	struct Context;
}
namespace grid {
	
	struct Grid:ecs::resource {
		//total length in one dimention
		
	//how many chunks it spans from the center chunk 
		const int rad;
		int dim_axis;
		 int totalChunks;
		v3::Coord gridpos;
		v3::Coord griddt;
		bool has_loaded_chunk;
		stn::array<Chunk::chunk*> chunklist;
		CtxName::Context* ctx;
		stn::queue<Coord> to_load;

		Grid():rad(0),totalChunks(0),chunklist(){
			ctx = nullptr;
		}
		Grid(int axis, CtxName::Context* Context);
		

		bool haschanged();
		void load();
		void updateborders();
		void destroy();


		Chunk::chunk*& operator[](int index) {
			return chunklist[index];
		}

		Point3 toBlockPos(Point3 point);
	Coord chunkfromblockpos(Coord pos);
	Coord getVoxel(Point3 pos);
	Coord get_chunk(Point3 pos);
	bool ChunkLoaded(Coord loc);
	bool containsChunk(Coord loc) const;
	int localChunkIndex(Coord NormedChunk);
	int chunkIndex(Coord Chunk);
	Chunk::chunk* GetChunk(Coord pos);

	stn::Option<Chunk::chunk&> get_chunk(Coord pos);
	 Option< ecs::obj&> get_object(const v3::Coord pos);

	 Option<block&> get_block(const v3::Coord pos);
	block* getBlock(const v3::Coord pos);
	bool containsChunkIndex(int index) const;
	ecs::obj* getObject(const v3::Coord pos);
	array<block*> voxelinrange(geometry::Box span);
	
	

	
	//normed pos is when pos is in the range [0...2*CtxName::cxt.Grid->length+1) for each direction;
	private:
		ChunkLoader loader;

	};
	
	
};

