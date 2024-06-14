#include "vector3.h"
#include "renderer.h"
#include <glm/glm.hpp>
#include "aabb.h"
#ifndef block_HPP
#define block_HPP
using namespace v3;
namespace block {
	enum id
	{
		minecraftair = 0,
	minecraftgrass = 1,
	};
	
	struct rblock
	{
		int texture;
		int id;
		aabb::colrect collider;
		Vector3 pos;
		 virtual void render();
		 rblock(Vector3 placment,int setid);
		 rblock();
		 void createaabb();
	};
	struct grass :rblock {

		int id = minecraftgrass;
		 void render() override;

	};
}


#endif // !block_H
