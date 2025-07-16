#include "../game/camera.h"
#include "chunkload.h"
#include "../util/vector3.h"
#include "../util/geometry.h"
#pragma once
namespace CtxName {
	struct Context;
}
namespace grid {
	
	struct Grid {
		//total length in one dimention
		inline int dim() {
			return 2 * rad + 1;
		};
	//how many chunks it spans from the center chunk 

		const int rad;
		const int totalChunks;
		v3::Coord gridpos;
		v3::Coord griddt;
		bool has_loaded_chunk;
		Cont::array<Chunk::chunk*> chunklist;
		CtxName::Context* ctx;
		Cont::queue<Coord> to_load;

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

	Vec3 toBlockPos(Vec3 point);
	Coord chunkfromblockpos(Coord pos);
	Coord getVoxel(Vec3 pos);

	bool ChunkLoaded(Coord loc);
	bool containsChunk(Coord loc);
	int localChunkIndex(Coord NormedChunk);
	int chunkIndex(Coord Chunk);
	Chunk::chunk* GetChunk(Coord pos);
	block* getBlock(const v3::Coord pos);
	array<block*>& voxelinrange(geometry::Box span);
	
	

	
	//normed pos is when pos is in the range [0...2*CtxName::cxt.Grid->length+1) for each direction;
	private:
		ChunkLoader loader;

	};
	
	
};

