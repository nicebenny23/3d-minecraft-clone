#include "../world/grid.h"
#include "../util/algorthm.h"
#pragma once 
namespace blockrender {
	void renderblocks(bool transparent);
	void initblockrendering();
	//for final boss
	extern bool enablelighting;
}

 // !blockrender_HPP

void renderchunk(Chunk::chunkmesh& mesh, bool transparent);
