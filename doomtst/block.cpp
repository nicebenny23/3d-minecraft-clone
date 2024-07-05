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
		model = glm::translate(model, (v3::vec(pos).glm()));




	}

}


block::block(v3::Coord placment, int setid)
{
	
	guid = gameobject::getgoid();


	complist = (array<gameobject::component*>());

	//ddcomponent<gameobject::component>();
	gameobject::objectfromguid[guid] = this;
	type = gameobject::block;
	id = setid;
	pos = placment;
	
	tex = facetex();
}

block::block()
{
	

	id = minecraftair;
	pos = v3::zeroiv;
	tex = facetex(minecraftair);
}

void block::createaabb()
{
	this->addcomponent<aabb::colrect>(v3::Vector3( pos )+ unitv / 2, unitv / 2, true);
	
}

void blockname::setair(blockname::block* blk)
{
	switch (blk->id)
	{
	case minecraftair:
		break;
	case minecraftgrass:
		

		blk->removecomponent<aabb::colrect>();
		break;
	case minecraftdirt:
		

		blk->removecomponent<aabb::colrect>();
		break;
	case minecraftstone:
		

		blk->removecomponent<aabb::colrect>();
		break;
	
	case minecraftglass:


		blk->removecomponent<aabb::colrect>();
		break;
	case minecraftwater:


		blk->removecomponent<aabb::colrect>();
		break;
    }
	blk->id = minecraftair;
	blk->tex = facetex(minecraftair);
}

void blockname::giveblocktraits(blockname::block* nullblock)
{
	switch (nullblock->id)
	{
	case minecraftair:
		break;
	case minecraftgrass:
		

		nullblock->createaabb();
		break;
	case minecraftdirt:
		

		nullblock->createaabb();
		break;
	case minecraftstone:
		
		nullblock->createaabb();
		break;
	case minecraftglass:

		nullblock->createaabb();
		break;
	case minecraftwater:

		nullblock->createaabb();
		break;
}
	nullblock->tex = facetex(nullblock->id);
}
