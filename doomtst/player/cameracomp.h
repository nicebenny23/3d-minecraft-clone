#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../game/entity.h"
#pragma once
struct CameraComp :gameobject::component
{
	CameraComp() {

		utype = gameobject::updaterender;
	}
	
	Transform CamTransform;
	void start() {
		CamTransform = objutil::toent(owner).transform;
		priority = 10000;

	}
	void update() {


		renderer::setviewmatrix(LookAt(CamTransform));
		
		CamTransform = objutil::toent(owner).transform;

	}
};
