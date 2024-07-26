#include "../block/block.h"
#include "../renderer/chunkrender.h"
#ifndef Chunk_HPP
#define Chunk_HPP
#define chunksize 16*16*16
using namespace blockname;
namespace Chunk {

	struct chunk;
	struct chunkmesh
	{
		chunkmesh() = default;
		vbuf VBO;
		vbuf ibo;
		vao Voa;
		//voa vertexspec;
		void genbufs();

		bool meshrecreateneeded;
		chunk* aschunk;
		array<float> datbuf;
		array<unsigned int> indbuf;
		array<face> facebuf;
		void sortbuf();

		//todo  get basic mesh and        reupdsted working


		void destroy();
	};
	const char* getcorefilename(Coord pos);
	int indexfrompos(int x, int y, int z);
	struct chunk
	{
		bool init;
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
	chunk* airload(Coord location);

	void createchunkmesh(chunk* aschunk);
}

#endif // !Chunk_H
#pragma once
