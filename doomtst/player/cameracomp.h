#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../math/mathutil.h"
#include "../game/entity.h"
#pragma once
inline double wrap_angle(double a) {
return wrap_to_range(a,0,360);
}
struct CameraComp : ecs::component
{
	CameraComp():CamTransform(Point3(0,0,0), 0, 0, unit_scale) {

		//utype = ecs::updaterender;
	}
	
	math::Transform CamTransform;
	void start() {
		CamTransform = owner().get_component<ecs::transform_comp>().transform;
	
	}
	void update() {
		owner().get_component<ecs::transform_comp>().transform.yaw=wrap_angle(owner().get_component<ecs::transform_comp>().transform.yaw);

		owner().get_component<ecs::transform_comp>().transform.pitch = wrap_to_range(owner().get_component<ecs::transform_comp>().transform.pitch,-90,90);
		CtxName::ctx.Ren->set_uniform("view_matrix", LookAt(owner().get_component<ecs::transform_comp>().transform));
		
		CamTransform = owner().get_component<ecs::transform_comp>().transform;

	}
};
