#include "block.h"
#include "../util/vector2.h"


using namespace blockname;
Vector3 face::center()
{
	return dirfromint(facenum) * mesh->scale + mesh->pos;
}
void face::calccameradist() {
	cameradist = dist2((*this).center(), Vector3(camera::campos));
}

void blockmesh::setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface) {
	left = face(leftface, 0, this);
	right = face(rightface, 1, this);
	up = face(upface, 2, this);
	down = face(downface, 3, this);
	front = face(frontface, 4, this);
	back = face(backface, 5, this);


}



face& blockmesh::operator[](int index)
{


	switch (index)
	{
	case 0:
		return left;
	case 1:
		return right;

	case 2:
		return up;
	case 3:
		return down;
	case 4:
		return front;
	case 5:
		return back;

	default:
		Assert("index too high/low for faces");

		break;

	}

}

void blockname::block::createfaces()
{
	switch (id)
	{
	case minecraftdirt:
		mesh->setfaces(0, 0, 0, 0, 0, 0);


		break;
	case minecraftgrass:
		mesh->setfaces( 0, 0, 1, 0, 0, 0);
		break;
	case minecraftstone:
		mesh->setfaces( 2, 2, 3, 3, 2, 2);
		break;
	case minecraftglass:
		mesh->setfaces( 4, 4, 4, 4, 4, 4);
		break;
	case minecraftwater:
		mesh->setfaces( 5, 5, 5, 5, 5, 5);
		break;
	case minecraftair:
		mesh->setfaces( -1, -1, -1, -1, -1, -1);
		break;
	}
}
void initblockmesh(blockname::block* blk, Vector3 pos,Vector3 scale) {
	blk->mesh =new blockmesh();
	
	
	blk->mesh->scale = scale;
	blk->mesh->pos = blk->center();
	blk->createfaces();
	blk->mesh->blk = blk;


}
blockname::block::block(v3::Coord placment, int blockid)
{
	
	guid = gameobject::getgoid();

	
	complist = (array<gameobject::component*>());
	//ddcomponent<gameobject::component>();
	gameobject::objectfromguid[guid] = this;
	type = gameobject::block;
	id = blockid;
	pos = placment;
	transparent = false;
	solid = true;
	initblockmesh(this, zerov, unitv / 2);

}

blockname::block::block()
{
	transparent = false;
	solid = false;
	
	id = minecraftair;
	pos = v3::zeroiv;
	
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
	default:
		Assert("need valid id in set blk");
    }
	blk->transparent = true;
	blk->solid =false;
	blk->id = minecraftair;

	blk->mesh->scale = unitv * 1 / 2;
}

void blockname::giveblocktraits(blockname::block* nullblock)
{
	nullblock->mesh->scale = unitv * 1 / 2;
	nullblock->solid = false;
	nullblock->transparent = true;
	return;
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
	nullblock->mesh->pos = (*nullblock).center();
	(*nullblock).createfaces();
}


void blockname::block::createaabb()
{
	this->addcomponent<aabb::colrect>(this->mesh->pos, this->mesh->scale, true);

}


