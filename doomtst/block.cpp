#include "block.h"
#include "vector2.h"
void block::rblock::render()
{
	
	if (id==minecraftair)
	{
		return;

		
	}
	if (id==minecraftgrass)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, 0.f, glm::vec3(1.f, 0.0f, 0.0f));
		model = glm::translate(model, (pos.glm()));
		
		
		

	}

}

block::rblock::rblock(Vector3 placment,int setid)
{
	id = setid;
	pos = placment;
	texture = 0;
}

block::rblock::rblock()
{
	texture = 0;
	id = minecraftair;
	pos = zerov;
	createaabb();
}

void block::rblock::createaabb()
{
	collider = aabb::colrect(pos+unitv/2, unitv/2, true);
}

