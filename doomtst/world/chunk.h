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
		

		renderer::RenderableHandle SolidGeo;
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
	std::string getcorefilename(Coord pos);
	size_t indexfrompos(Coord pos);
	struct chunk
	{
		bool init;
		Coord loc;
		chunkmesh* mesh;
		bool modified;
		void write();
		chunk();
		
		Vec3 center() const {
			return (loc+ unitv /2.f)*chunklength;
		}
		gameobject::obj& operator[](size_t index);
		gameobject::obj* blockbuf;
		void destroy();
		float cameradist() const {
			return 	 dist(center(), camera::campos());
		}
		bool operator<(const chunk& b) const {
			return b.cameradist() < cameradist();
		}
		bool operator>(const chunk& b) const {
			return b.cameradist() > cameradist();
		}

	};
	chunk* airload(Coord location);

	void createchunkmesh(chunk* aschunk);
}

 // !Chunk_H
#pragma once
