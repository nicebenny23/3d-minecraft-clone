#include "block.h"
#include "../util/vector2.h"


using namespace blockname;
Vector3 face::center()
{
	return dirfromint(facenum) * mesh->scale + mesh->pos;
}
void face::calccameradist() {
	cameradist = 128 * dist2((*this).center(), Vector3(camera::campos));
}

void blockmesh::setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface) {
	up = face(upface, 2, this);
	down = face(downface, 3, this);

	switch (direction)
	{
	case west2d:
		left = face(frontface, 0, this);
		right = face(backface, 1, this);
		front = face(rightface, 4, this);
		back = face(leftface, 5, this);
		break;
	case east2d:
		left = face(backface, 0, this);
		right = face(frontface, 1, this);
		front = face(leftface, 4, this);
		back = face(rightface, 5, this);
		break;
	case front2d:
		left = face(leftface, 0, this);
		right = face(rightface, 1, this);
		front = face(frontface, 4, this);
		back = face(backface, 5, this);
		break;
	case back2d:
		left = face(rightface, 0, this);
		right = face(leftface, 1, this);
		front = face(backface, 4, this);
		back = face(frontface, 5, this);
		break;

	}





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

void blockname::blockmesh::attachindirection()
{

	Vector3 maxpos = blk->center() + scale * dirfromint(attachdir);
	Vector3 blkpos = dirfromint(attachdir) / 2 + blk->center();
	pos += blkpos - maxpos;
}
face& blockname::block::operator[](int index)
{
	return (mesh)[index];
}

void blockname::initblockmesh(blockname::block* blk, Vector3 pos, Vector3 scale) {




	blk->mesh = blockmesh();
	blk->mesh.scale = scale;
	blk->mesh.pos = blk->center();
	blk->mesh.blk = blk;
	blk->mesh.direction = front2d;
	blk->mesh.attachdir = 0;

}
//initiates block as gameobject;
blockname::block::block(v3::Coord location, int blockid)
{

	
	emitedlight = 0;
	complist = (array<gameobject::component*>());
	type = gameobject::block;
	mininglevel = 0;
	id = blockid;
	pos = location;
	transparent = false;
	solid = true;


}

blockname::block::block()
{
	transparent = false;
	solid = false;
	complist = array<gameobject::component*>();
	id = -1;
	pos = v3::zeroiv;
	type = gameobject::block;


}

void blockname::block::initasgameobj()
{
	transparent = false;
	solid = false;

	emitedlight = 0;
	complist = (array<gameobject::component*>());
	type = gameobject::block;




}



void blockname::block::createaabb()
{
	this->addcomponent<aabb::Collider>(this->mesh.pos, this->mesh.scale, false,!solid);

}


