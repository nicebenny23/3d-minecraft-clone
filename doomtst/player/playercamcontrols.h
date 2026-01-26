#include "../game/entity.h"
#include "../game/entitystate.h"
#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/entityutil.h"
#include "../items/menu.h"
#pragma once
struct camera_event {
	ecs::obj object_viewed;
	camera_event(ecs::obj viewed) :object_viewed(viewed) {
	}
};
struct playercamcontrols : ecs::component
{
	void update() {


		if (!world().ensure_resource<ui::MenuState>().menu_open())
		{
			CtxName::ctx.Window->DisableCursor();

			double xoffset = CtxName::ctx.Inp->mouse_position_dt.x;
			double yoffset = CtxName::ctx.Inp->mouse_position_dt.y;

			float sensitivity = 60;

			if (abs(xoffset) > 180)
			{
				xoffset = 0;
			}  if (abs(yoffset) > 180)
			{
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			owner().get_component<ecs::transform_comp>().transform.pitch += yoffset;

			owner().get_component<ecs::transform_comp>().transform.yaw += xoffset;

			owner().get_component<ecs::transform_comp>().transform.pitch = clamp(owner().get_component<ecs::transform_comp>().transform.pitch, -89.99, 89.99);

		}
		else
		{
			CtxName::ctx.Window->EnableCursor();
		}
		ray cameraray = ray::from_offset(owner().get_component<ecs::transform_comp>().transform.position,owner().get_component<ecs::transform_comp>().transform.normal_dir() * interactmaxrange);
		voxtra::WorldRayCollision closest = collision::raycastall(cameraray, collision::HitQuery());
		if (closest)
		{
			owner().world().write_event<camera_event>(closest.unwrap().owner());
		}

	}
};
