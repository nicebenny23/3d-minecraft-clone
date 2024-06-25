#include "block.h"
#include "vector2.h"


using namespace blockname;

void block::render()
{

	if (id == minecraftair)
	{
		return;


	}
	if (id == minecraftgrass)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, 0.f, glm::vec3(1.f, 0.0f, 0.0f));
		model = glm::translate(model, (pos.glm()));




	}

}


block::block(iv3::Ivector3 placment, int setid)
{
	
	guid = gameobject::getgoid();


	complist = (array<gameobject::component*>());

	//ddcomponent<gameobject::component>();
	gameobject::objectfromguid[guid] = this;
	type = gameobject::block;
	id = setid;
	pos = placment;
	texture = 0;

}

block::block()
{
	
	texture = 0;
	id = minecraftair;
	pos = iv3::zerov;
	
}

void block::createaabb()
{
	this->addcomponent<aabb::colrect>(v3::Vector3( pos )+ unitv / 2, unitv / 2, true);
	
}
