#include "chunk.h"

#ifndef chunkload_HPP
#define chunkload_HPP


Chunk::chunk* chunkfileload(Coord location);

int generatechunkvalfromnoise(float noiselevel, Coord position, float feturemap, float modulatedmap, float biomemap,float map2);
 Chunk::chunk* chunkload(Coord location);

#endif // chunkload_HPP
