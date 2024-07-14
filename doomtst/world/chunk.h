#include "../block/block.h"
#include "../renderer/chunkrender.h"
#ifndef Chunk_HPP
#define Chunk_HPP
#define chunksize 16*16*16
using namespace blockname;
namespace Chunk {
	const char* getcorefilename(Coord pos);
	int indexfrompos(int x, int y, int z);
	struct chunk
	{
		
		Coord loc;
		chunkmesh* mesh;
		bool modified;
		void write();
		chunk();
		Coord center() {
			return loc*16 + unitv * 8;
		}
		block& operator[](int index);
		block* blockbuf;
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
	chunk* fileload(Coord location);
}
#endif // !Chunk_H
#pragma once
