
#include "renderer.h"
#include <glm/glm.hpp>
#include "gameobject.h"
#include "aabb.h"
#ifndef block_HPP
#define block_HPP
using namespace v3;
namespace blockname {
	enum id
	{
		minecraftair = 0,
	minecraftgrass = 1,
	minecraftdirt=2,
	};
	
	struct block:gameobject::obj
	{
		int texture;
		int id;
		iv3::Ivector3 pos;
		
	
 void render();
		 block(iv3::Ivector3 placment,int setid);
		 block();
		 void createaabb();
	};
	
}


#endif // !block_H
