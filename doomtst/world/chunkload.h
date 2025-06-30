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
	
	Chunk::chunk* LoadChunk(Coord location);
	Chunk::chunk* AllocChunk(Coord location);
	Chunk::chunk* LoadFromFile(Coord location);
	Chunk::chunk* LoadFromNoise(Coord location);
private:
	grid::Grid* Grid;
};
 // chunkload_HPP
