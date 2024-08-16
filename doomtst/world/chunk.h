#include "../block/block.h"
#include "../renderer/chunkrender.h"
#ifndef Chunk_HPP
#define Chunk_HPP

constexpr int chunklength = 16;
constexpr int chunkaxis =chunklength/blocksize;
constexpr auto chunksize = chunkaxis*chunkaxis*chunkaxis;
using namespace blockname;
namespace Chunk {

	struct chunk;
	struct chunkmesh
	{
		chunkmesh() = default;
		vbuf VBO;
		vbuf ibo;

		vao Voa;
		vbuf transparentVBO;
		vbuf transparentibo;
		vao transparentVoa;
		//voa vertexspec;
		void genbufs();
		int meshsize;
		bool meshrecreateneeded;
		chunk* aschunk;
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
		
		Vector3 center() {
			return (loc+ unitv /2.f)*chunklength;
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
