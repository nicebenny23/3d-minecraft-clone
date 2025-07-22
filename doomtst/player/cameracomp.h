#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../util/mathutil.h"
#include "../game/entity.h"
#pragma once
inline double wrap_angle(double a) {
return wrap_to_range(a,0,360);
}
struct CameraComp : gameobject::component
{
	CameraComp() {

		utype = gameobject::updaterender;
	}
	
	Transform CamTransform;
	void start() {
		CamTransform = owner.transform();
		priority = 10000;

	}
	void update() {
		owner.transform().yaw=wrap_angle(owner.transform().yaw);

		owner.transform().pitch = wrap_to_range(owner.transform().pitch,-90,90);
		CtxName::ctx.Ren->set_uniform("view_matrix", LookAt(owner.transform()));
		
		CamTransform = owner.transform();

	}
};
