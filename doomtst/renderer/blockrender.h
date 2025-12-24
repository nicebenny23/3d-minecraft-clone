#include "../world/grid.h"
#include "../util/algorthm.h"
#pragma once 
namespace blockrender {
	void initblockrendering();
	//for final boss
	extern bool enablelighting;
	struct ChunkMesher :ecs::System {
		ChunkMesher() {

		}

		void run(ecs::Ecs& ecs);
	};
	struct BlockRenderer:ecs::System {
		BlockRenderer() {

		}

		void run(ecs::Ecs& ecs);
	};
}

 // !blockrender_HPP

void renderchunk(Chunk::chunkmesh& mesh, bool transparent);
