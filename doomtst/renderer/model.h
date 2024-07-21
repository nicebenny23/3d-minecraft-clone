#include "mesh.h"
#include "../game/gameobject.h"
#ifndef model_HPP
#define model_HPP

struct model:gameobject::component
{
	void draw();
	model();
	float yaw;
	float pitch;
	array<meshname::mesh> meshlist;
};


#endif // !modelname
