#include "block.h"
#include "vector2.h"


using namespace blockname;


block::block(v3::Coord placment, int blockid)
{
	
	guid = gameobject::getgoid();


	complist = (array<gameobject::component*>());

	//ddcomponent<gameobject::component>();
	gameobject::objectfromguid[guid] = this;
	type = gameobject::block;
	id = blockid;
	pos = placment;
	
	createfaces(this);
}

block::block()
{
	

	id = minecraftair;
	pos = v3::zeroiv;
	createfaces(this);
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

		
		
		break;
    }
	blk->transparent = true;
	blk->solid =false;
	blk->id = minecraftair;
	createfaces(blk);
}

void blockname::giveblocktraits(blockname::block* nullblock)
{
	switch (nullblock->id)
	{
	case minecraftair:
		nullblock->solid = false;
		nullblock->transparent = true;
		break;
	case minecraftgrass:
		nullblock->transparent = false;
		nullblock->solid = true;
		nullblock->createaabb();
		break;
	case minecraftdirt:
		nullblock->solid = true;

		nullblock->transparent = false;
		nullblock->createaabb();
		break;
	case minecraftstone:
		nullblock->solid = true;
		nullblock->transparent = false;
		nullblock->createaabb();
		break;
	case minecraftglass:
		nullblock->solid = true;
		nullblock->transparent = true;
		nullblock->createaabb();
		break;
	case minecraftwater:
		nullblock->solid = false;

		nullblock->transparent = true;
		break;
}
	createfaces(nullblock);
}
void setfaces(block* blk,int frontface, int backface, int leftface, int rightface, int upface, int downface) {

	blk->front = face(frontface, 0, blk);
	blk->back = face(backface, 1, blk);

	blk->left = face(leftface, 2, blk);
	blk->right = face(rightface, 3, blk);
	blk->up = face(upface, 4, blk);
	blk->down = face(downface, 5, blk);
}
void blockname::createfaces(block*blk)
{
	switch (blk->id)
	{
	case minecraftdirt:
		setfaces(blk,0, 0, 0, 0, 0, 0);
	
		
		break;
	case minecraftgrass:
		setfaces(blk,0, 0, 0, 0, 1, 0);
		break;
	case minecraftstone:
		setfaces(blk,2, 2, 2, 2, 3, 3);
		break;
	case minecraftglass:
		setfaces(blk,4,4,4,4,4,4);
		break;
	case minecraftwater:
		setfaces(blk, 5, 5, 5, 5, 5, 5);
		break;
	case minecraftair:
		setfaces(blk, -1,-1,-1,-1,-1,-1);
		break;
	}
}

blockname::face::face()
{
	tex = -1;
	cameradist = -1;
	holder = nullptr;
	facenum = -1;
	covered = false;	
}

blockname::face::face(byte texval, int num, block* owner)
{
	tex = texval;
	facenum = num;
	holder = owner;
	cameradist = -1;
	calccameradist(this);
	covered = false;
}


