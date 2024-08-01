#include "../world/grid.h"
#include "algorthm.h"
#ifndef blockrender_HPP
#define blockrender_HPP
namespace blockrender {
	void setrendertransparent();
	extern texturearray texarray;
	void setrendersolid();
	void renderblocks(bool transparent);
	void initblockrendering();
	//for final boss
	extern bool enablelighting;
}

#endif // !blockrender_HPP

void renderchunk(Chunk::chunkmesh& mesh, bool transparent);
