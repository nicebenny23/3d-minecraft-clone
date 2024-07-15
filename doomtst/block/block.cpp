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

void blockname::initblockmesh(blockname::block* blk, Vector3 pos,Vector3 scale) {
	
	
	
	blockmesh* mesh=new blockmesh();


	mesh->scale = scale;
	mesh->pos = blk->center();
	
	mesh->blk = blk;
	blk->mesh = mesh;
	
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



void blockname::block::createaabb()
{
	this->addcomponent<aabb::colrect>(this->mesh->pos,this->mesh->scale, true);

}


