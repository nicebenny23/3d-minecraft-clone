#include "block.h"

#ifndef Chunk_HPP
#define Chunk_HPP
#define chunksize 16*16*16
using namespace blockname;
namespace chunk {
	
	inline int indfromlocalpos(int x, int y, int z);
	int indexfrompos(int x, int y, int z);
	struct chunk
	{
		Coord loc;
		void renderchunk();
		chunk();
		
		block* blockstruct;
		void destroy();
	};
	
	chunk* load(Coord location);
}
#endif // !Chunk_H
#pragma once
