#include "../block/block.h"
#include "../util/cached.h"
#include "chunk_mesh.h"
#pragma once 
using namespace blocks;
namespace Chunk {

	constexpr size_t chunkaxis = size_t(chunklength / blocksize);
	constexpr size_t chunksize = chunkaxis * chunkaxis * chunkaxis;
	std::string getcorefilename(Coord pos);
	size_t indexfrompos(Coord pos);
	
	struct chunk:ecs::component
	{
		Coord loc;
		bool modified;
		void write();
		chunk(v3::Coord location) :modified(false), loc(location), block_list(chunksize) {

		}
		Point3 center() const {
			return (loc+ unitv /2.f)*chunklength;
		}
		ecs::obj& operator[](size_t index) {
			return block_list[index];
		}
		const ecs::obj& operator[](size_t index) const{
			return block_list[index];
		}
		stn::array<ecs::obj> block_list;
		void destroy();
	};

}

 // !Chunk_H
#pragma once
