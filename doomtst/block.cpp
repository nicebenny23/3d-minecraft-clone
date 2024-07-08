#include "block.h"
#include "vector2.h"


using namespace blockname;
void setfaces(block* blk, int leftface, int rightface, int upface, int downface, int frontface, int backface) {
	blk->left = face(leftface, 0, blk);
	blk->right = face(rightface, 1, blk);
	blk->up = face(upface, 2, blk);
	blk->down = face(downface, 3, blk);
	blk->front = face(frontface, 4, blk);
	blk->back = face(backface, 5, blk);


}

void createfaces(block* blk)
{
	switch (blk->id)
	{
	case minecraftdirt:
		setfaces(blk, 0, 0, 0, 0, 0, 0);


		break;
	case minecraftgrass:
		setfaces(blk, 0, 0, 1, 0, 0, 0);
		break;
	case minecraftstone:
		setfaces(blk, 2, 2, 3, 3, 2, 2);
		break;
	case minecraftglass:
		setfaces(blk, 4, 4, 4, 4, 4, 4);
		break;
	case minecraftwater:
		setfaces(blk, 5, 5, 5, 5, 5, 5);
		break;
	case minecraftair:
		setfaces(blk, -1, -1, -1, -1, -1, -1);
		break;
	}
}

block::block(v3::Coord placment, int blockid)
{
	
	guid = gameobject::getgoid();


	complist = (array<gameobject::component*>());

	//ddcomponent<gameobject::component>();
	gameobject::objectfromguid[guid] = this;
	type = gameobject::block;
	id = blockid;
	pos = placment;
	
	createfaces(t
his);
	blkoffset = zerov;
}

block::block()
{
	
	scale = unitv / 2;
	blkoffset = zerov;
	id = minecraftair;
	pos = v3::zeroiv;
	createfaces(this);
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
	blk->scale = unitv * 1 / 2;
}

void blockname::giveblocktraits(blockname::block* nullblock)
{
	nullblock->scale = unitv * 1 / 2;
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
		nullblock->scale = unitv*1/ 3;
		break;
	case minecraftglass:
		nullblock->solid = true;
		nullblock->transparent = true;
		
		nullblock->scale = unitv * 1 /2;
		nullblock->blkoffset = Vector3(1,0,0) ;
		nullblock->createaabb();
		break;
	case minecraftwater:
		nullblock->solid = false;

		nullblock->transparent = true;
		break;

	createfaces(nullblock);
}

blockname::face::face()
{
	tex = -1;
	cameradist = -1;
	holder = nullptr;
	facenum = -1;
	covered = false;	
}

void face::calccameradist()
{
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

Vector3 blockname::face::center()
{
	v3::Vector3	offset = dirfromint(facenum) * holder->scale/2 * .9999;
	return((holder)->center() );
}
void block::createaabb()
{
	this->addcomponent<aabb::colrect>(this->center(), scale, true);

}

block blockname::block(v3::Coord placment, int blockid)
{
	return block();
}

void blockname::block::createfaces()
{
}
