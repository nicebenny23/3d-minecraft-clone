#include "chunk.h"
#ifndef chunkload_HPP
#define chunkload_HPP
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
#endif // chunkload_HPP
