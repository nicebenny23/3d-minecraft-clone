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
	//0,left
//1,right
//2,up
//3,down
//4,front
//5,back
	faces[2] = face(upface, 2, this);
	faces[3] = face(downface, 3, this);

	switch (direction)
	{
	case west2d:
		faces[0] = face(frontface, 0, this);
		faces[1] = face(backface, 1, this);
		faces[4] = face(rightface, 4, this);
		faces[5] = face(leftface, 5, this);
		break;
	case east2d:
		faces[0] = face(backface, 0, this);
		faces[1] = face(frontface, 1, this);
		faces[4] = face(leftface, 4, this);
		faces[5] = face(rightface, 5, this);
		break;
	case front2d:
		faces[0] = face(leftface, 0, this);
		faces[1] = face(rightface, 1, this);
		faces[4] = face(frontface, 4, this);
		faces[5] = face(backface, 5, this);
		break;
	case back2d:
		faces[0] = face(rightface, 0, this);
		faces[1] = face(leftface, 1, this);
		faces[4] = face(backface, 4, this);
		faces[5] = face(frontface, 5, this);
		break;

	}





}



face& blockmesh::operator[](int index)
{

	switch (index)
	{
	case 0:

	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		return faces[index];
		break;
	default:
		Assert("index too high/low for faces");

		break;

	}

}

void blockname::blockmesh::attachindirection()
{

	Vector3 maxpos = blk->center() + scale * dirfromint(attachdir);
	Vector3 blkpos = dirfromint(attachdir)*blocksize / 2 + blk->center();
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

	state = gameobject::beinginitiated;
	emitedlight = 0;
	
	//componentlist = (array<gameobject::component*>(2,false));
	type = gameobject::block;
	mininglevel = 0;
	id = blockid;
	pos = location;
	
	
	
	attributes.transparent = false;
	attributes.solid = true;


}

blockname::block::block()
{

	state = gameobject::beinginitiated;
	attributes.transparent = false;
	attributes.solid = false;

	id = -1;
	pos = v3::zeroiv;
	type = gameobject::block;


}



void blockname::block::createaabb(bool effector)
{
	addcomponent<aabb::Collider>(this->mesh.pos, this->mesh.scale, false,effector);

}


