#include "block.h"

#ifndef Chunk_HPP
#define Chunk_HPP
using namespace block;
namespace chunk {
	inline int indfrompos(int x, int y, int z);
	struct chunk
	{
		int xchunk;
		int zchunk;
		void renderchunk();
		chunk();
		
		rblock* blockstruct;
		void destroy();
	};
	chunk* load(int xind, int zind);
}
#endif // !Chunk_H
#pragma once
