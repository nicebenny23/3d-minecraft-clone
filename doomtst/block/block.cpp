#include "block.h"
#include "../util/vector2.h"


using namespace blockname;
Vector3 face::center()
{
	return dirfromint(facenum) * mesh->scale + mesh->pos;
}
void face::calccameradist() {
	cameradist = 128*dist2((*this).center(), Vector3(camera::campos));
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

face& blockname::block::operator[](int index)
{
	return (*mesh)[index];
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
		mesh->setfaces( 6,6,7,4,6,6);
		break;
	case minecraftwater:
		mesh->setfaces( 5, 5, 5, 5, 5, 5);
		break;
	case minecraftair:
		mesh->setfaces( -1, -1, -1, -1, -1, -1);
		break;
	}
}
void blockname::initblockmesh(blockname::block* blk, Vector3 pos,Vector3 scale) {
	
	
	
	blockmesh* mesh=new blockmesh();


	mesh->scale = scale;
	mesh->pos = blk->center();
	
	mesh->blk = blk;
	blk->mesh = mesh;
	blk->createfaces();
}
blockname::block::block(v3::Coord placment, int blockid)
{
	
	guid = gameobject::getgoid();

	emitedlight = 0;
	 complist = (array<gameobject::component*>());
	type = gameobject::block;
	
	gameobject::objectfromguid[guid] = this;
	
	id = blockid;
	pos = placment;
	transparent = false;
	solid = true;
	

}

blockname::block::block()
{
	transparent = false;
	solid = false;
	//complist = array<gameobject::component*>();
	id = -1;
	guid = -1;
	pos = v3::zeroiv;
	type = gameobject::block;
	test = false;
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
	blk->emitedlight = 0;
	blk->lightval= 0;
	blk->mesh->scale = unitv * 1 / 2.01;
}

void blockname::giveblocktraits(blockname::block* nullblock)
{
	nullblock->mesh->scale = unitv * 1 / 2.001;
	nullblock->emitedlight =0;
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
		nullblock->emitedlight = 15;
	nullblock->mesh->scale = v3::Vector3(1/16.f,.375,1/16.f);
		
		nullblock->createaabb();
		break;
	case minecraftwater:
		nullblock->solid = false;

		nullblock->transparent = true;
		break;

		
	}
	//nullblock->mesh->scale = unitv * 1 / 2.01;
	(*nullblock).createfaces();
}


void blockname::block::createaabb()
{
	this->addcomponent<aabb::colrect>(this->mesh->pos,this->mesh->scale, true);
	int l = 1;
}


