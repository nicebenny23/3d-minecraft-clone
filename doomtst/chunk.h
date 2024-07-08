#include "block.h"
#include "chunkrender.h"
#ifndef Chunk_HPP
#define Chunk_HPP
#define chunksize 16*16*16
using namespace blockname;
namespace Chunk {
	
	inline int indfromlocalpos(int x, int y, int z);
	int indexfrompos(int x, int y, int z);
	struct chunk
	{
		Coord loc;
		chunkmesh* mesh;
		
		chunk();
		Coord center() {
			return loc*16 + unitv * 8;
		}
	
		block* blockstruct;
		void destroy();
		float cameradist() {
			return 	 distance(center(), camera::campos);
		}
		bool operator<(chunk& b) {
			return b.cameradist() < cameradist();
		}
		bool operator>(chunk& b) {
			return b.cameradist() > cameradist();
		}

	};
	
	chunk* load(Coord location);
}
#endif // !Chunk_H
#pragma once
