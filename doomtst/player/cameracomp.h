#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../util/mathutil.h"
#include "../game/entity.h"
#pragma once
inline double wrap_angle(double a) {
return std::fmod(std::fmod(a, 360.0) + 360.0, 360.0);
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
		CamTransform.yaw=wrap_angle(CamTransform.yaw);

		CamTransform.pitch = wrap_angle(CamTransform.pitch);
		CtxName::ctx.Ren->setviewmatrix(LookAt(CamTransform));
		
		CamTransform = owner.transform();

	}
};
