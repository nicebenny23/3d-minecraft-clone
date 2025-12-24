#include "chunk.h"
#pragma once 
namespace grid{
	struct Grid;
}
namespace CtxName {
	struct Context;
}
struct ChunkLoader
{
	void Init(CtxName::Context* ctx);
	
	Chunk::chunk& LoadChunk(Coord location);
	Chunk::chunk& AllocChunk(Coord location);	
private:
	Chunk::chunk& LoadFromFile(Coord location);
	Chunk::chunk& LoadFromNoise(Coord location);
	grid::Grid* Grid;
};
 // chunkload_HPP
