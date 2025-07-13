#include "../block/block.h"
#include "../renderer/chunkrender.h"
#pragma once 
constexpr int chunklength = 16;
constexpr int chunkaxis =chunklength/blocksize;
constexpr int chunksize = chunkaxis*chunkaxis*chunkaxis;
using namespace blockname;
namespace Chunk {

	struct chunk;
	struct chunkmesh
	{
		chunkmesh() = default;
		
		Mesh SolidGeo;
		Mesh TransparentGeo;
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
	int indexfrompos(Coord pos);
	struct chunk
	{
		bool init;
		Coord loc;
		chunkmesh* mesh;
		bool modified;
		void write();
		chunk();
		
		Vec3 center() {
			return (loc+ unitv /2.f)*chunklength;
		}
		block& operator[](int index);
		gameobject::obj* blockbuf;
		void destroy();
		float cameradist() {
			return 	 dist(center(), camera::campos());
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

 // !Chunk_H
#pragma once
