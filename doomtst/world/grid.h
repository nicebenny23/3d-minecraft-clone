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
		v3::Vector3 gridpos;
		v3::Vector3 griddt;
		dynamicarray::array<Chunk::chunk*> chunklist;
		CtxName::Context* ctx;


		Grid():rad(0),totalChunks(0),chunklist(0){
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

	Vector3 scaletoblocksize(Vector3 point);
	Coord chunkfromblockpos(Coord pos);
	Coord getvoxellocation(Vector3 pos);

	bool chunkInBounds(Coord loc);
	int gridindexfromnormedchunkpos(Coord NormedChunk);
	int gridindfromchunkpos(Coord Chunk);
	Chunk::chunk* GetChunk(Coord pos);
	block* getBlock(const v3::Coord pos);
	array<block*>& voxelinrange(geometry::Box span);
	
	

	
	//normed pos is when pos is in the range [0...2*CtxName::cxt.Grid->length+1) for each direction;
	private:
		ChunkLoader loader;

	};
	
	
};

