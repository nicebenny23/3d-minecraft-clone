#include "../debugger/debug.h"
#include "block.h"
#include "../util/vector2.h"
#include "../game/aabb.h"

using namespace blockname;
Vec3 face::center()
{
	return Vec3( facenum.ToVec())* mesh->box.scale + mesh->box.center;
}
void face::calccameradist() {
	cameradist = 128 * dist2((*this).center(), Vec3(camera::campos()));
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

	switch (Dir::Ind2d(direction.ind()))
	{
		
	case Dir::Ind2d::Left://west
		faces[0].create(frontface, 0, this);
		faces[1].create(backface, 1, this);
		faces[4].create(rightface, 4, this);
		faces[5].create(leftface, 5, this);
		break;
	case  Dir::Ind2d::Right://east
		faces[0].create(backface, 0, this);
		faces[1].create(frontface, 1, this);
		faces[4].create(leftface, 4, this);
		faces[5].create(rightface, 5, this);
		break;
	case Dir::Ind2d::Up ://block faces fowards
		faces[0].create(leftface, 0, this);
		faces[1].create(rightface, 1, this);
		faces[4].create(frontface, 4, this);
		faces[5].create(backface, 5, this);
		break;
		case Dir::Ind2d::Down ://block faces backwords
		faces[0].create(rightface, 0, this);
		faces[1].create(leftface, 1, this);
		faces[4].create(backface, 4, this);
		faces[5].create(frontface, 5, this);
		break;

	}





}



face& blockmesh::operator[](size_t index)
{
	if (index<6&&0<=index)
	{
		return faces[index];
	}

	
		Assert("index too high/low for faces");

}

void blockname::blockmesh::attachindirection()
{

	Vec3 maxpos = blk->center() + box.scale * (blk->mesh.attachdir.ToVec());
	Vec3 blkpos = Vec3(blk->mesh.attachdir.ToVec())*blocksize / 2 + blk->center();
	box.center += blkpos - maxpos;
}
face& blockname::block::operator[](size_t index)
{
	return (mesh)[index];
}

blockname::blockmesh::blockmesh(block* parent, Vec3 blkscale)
{
	blk = parent;
	direction = Dir::up2d;
	attachdir = Dir::front3d;

	box.scale = blkscale;

	box.center =Vec3(blk->pos+unitv/2) * blocksize;



}

void blockname::block::create(v3::Coord location, int blockid, Dir::Dir3d blkattachface, Dir::Dir2d blkdirection)
{

	emitedlight = 0;

	mininglevel = 0;
	id = blockid;
	pos = location;


	
	attributes.transparent = false;
	attributes.solid = true;

	mesh = blockname::blockmesh(this, blockscale);
	mesh.direction = blkdirection;

	if (mesh.direction.ind())
	{

	}
	mesh.attachdir = blkattachface;
}

void blockname::block::createdefaultaabb(bool effector)
{
	owner().add_component<aabb::Collider>(zerov, unitv/2.f, false,effector);

}
