#include "../block/block.h"
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
		chunkmesh(chunk& cnk) :recreate_mesh(false),owner(cnk){
		};
		

		renderer::RenderableHandle SolidGeo;
		renderer::RenderableHandle TransparentGeo;
		void genbufs();
	
		stn::dirty_flag recreate_mesh;
		chunk& owner;
		array<face> faces;
		
		void sort_faces();
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
	

	};

}

 // !Chunk_H
#pragma once
