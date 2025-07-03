
#include "block.h"
#include "../util/vector2.h"
#include "../game/aabb.h"

using namespace blockname;
Vector3 face::center()
{
	return Vector3( facenum.ToVec())* mesh->box.scale + mesh->box.center;
}
void face::calccameradist() {
	cameradist = 128 * dist2((*this).center(), Vector3(camera::campos()));
}

void blockmesh::setfaces(int leftface, int rightface, int upface, int downface, int frontface, int backface) {
	//0,left
//1,right
//2,up
//3,down
//4,front
//5,back
	
	faces[2].create(upface, 2, this);
	faces[3].create(downface, 3, this);

	switch (blk->mesh.direction)
	{
	case Dir::dir2d::west2d:
		faces[0].create(frontface, 0, this);
		faces[1].create(backface, 1, this);
		faces[4].create(rightface, 4, this);
		faces[5].create(leftface, 5, this);
		break;
	case Dir::dir2d::east2d:
		faces[0].create(backface, 0, this);
		faces[1].create(frontface, 1, this);
		faces[4].create(leftface, 4, this);
		faces[5].create(rightface, 5, this);
		break;
	case Dir::dir2d::front2d:
		faces[0].create(leftface, 0, this);
		faces[1].create(rightface, 1, this);
		faces[4].create(frontface, 4, this);
		faces[5].create(backface, 5, this);
		break;
	case Dir::dir2d::back2d:
		faces[0].create(rightface, 0, this);
		faces[1].create(leftface, 1, this);
		faces[4].create(backface, 4, this);
		faces[5].create(frontface, 5, this);
		break;

	}





}



face& blockmesh::operator[](int index)
{
	if (index<6&&0<=index)
	{
		return faces[index];
	}

	
		Assert("index too high/low for faces");

}

void blockname::blockmesh::attachindirection()
{

	Vector3 maxpos = blk->center() + box.scale * (blk->mesh.attachdir.ToVec());
	Vector3 blkpos = Vector3(blk->mesh.attachdir.ToVec())*blocksize / 2 + blk->center();
	box.center += blkpos - maxpos;
}
face& blockname::block::operator[](int index)
{
	return (mesh)[index];
}

blockname::blockmesh::blockmesh(block* parent, Vector3 blkscale)
{
	blk = parent;
	

	box.scale = blkscale;
	box.center = parent->center();


}

void blockname::block::create(v3::Coord location, int blockid, char blkattachface, char blkdirection)
{

	owner->state() = gameobject::beinginitiated;
	emitedlight = 0;

	owner->type() = gameobject::block;
	mininglevel = 0;
	id = blockid;
	pos = location;


	
	attributes.transparent = false;
	attributes.solid = true;

	mesh = blockname::blockmesh(this, blockscale);
	mesh.direction = (blkdirection);
	mesh.attachdir =Dir::Dir3d( blkattachface);
}

void blockname::block::createdefaultaabb(bool effector)
{
	owner->addcomponent<aabb::Collider>(zerov, unitscale, false,effector);

}
