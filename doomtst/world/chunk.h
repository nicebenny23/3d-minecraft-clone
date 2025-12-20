#include "../block/block.h"
#include "../renderer/chunkrender.h"
#include "../util/cached.h"
#pragma once 
constexpr int chunklength = 16;
constexpr int chunkaxis =int(chunklength/blocksize);
constexpr int chunksize = chunkaxis*chunkaxis*chunkaxis;
using namespace blockname;
namespace Chunk {

	struct chunk;
	struct chunkmesh
	{
		chunkmesh() :recreate_mesh(false){
		};
		

		renderer::RenderableHandle SolidGeo;
		renderer::RenderableHandle TransparentGeo;
		//voa vertexspec;
		void genbufs();
	
		stn::dirty_flag recreate_mesh;
		chunk* aschunk;
		array<face> facebuf;
		
		void sortbuf();
		void destroy();
	};
	std::string getcorefilename(Coord pos);
	size_t indexfrompos(Coord pos);
	struct chunk
	{
		bool init;
		Coord loc;
		stn::box<chunkmesh> mesh;
		bool modified;
		void write();
		chunk();
		
		Point3 center() const {
			return (loc+ unitv /2.f)*chunklength;
		}
		ecs::obj& operator[](size_t index);
		stn::array<ecs::obj> blockbuf;
		void destroy();
		double cameradist() const {
			return 	 dist(center(), camera::campos());
		}
		bool operator<(const chunk& b) const {
			return b.cameradist() < cameradist();
		}
		bool operator>(const chunk& b) const {
			return b.cameradist() > cameradist();
		}

	};

	void createchunkmesh(chunk* aschunk);
}

 // !Chunk_H
#pragma once
