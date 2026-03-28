
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

			world().get_resource<window::Window>().DisableCursor();

			userinput::InputManager& man = world().get_resource<userinput::InputManager>();
			double xoffset = man.mouse_position_dt.x;
			double yoffset = man.mouse_position_dt.y;

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

			owner().get_component<ecs::world_transform>().transform.pitch += yoffset;

			owner().get_component<ecs::world_transform>().transform.yaw += xoffset;

			owner().get_component<ecs::world_transform>().transform.pitch = clamp(owner().get_component<ecs::world_transform>().transform.pitch, -89.99, 89.99);

		}
		else
		{
			Core::game.Ecs.get_resource<window::Window>().EnableCursor();
		}
		double max_interact_range = 5;
		math::ray cameraray = owner().get_component<ecs::world_transform>().transform.forward_ray().with_length(max_interact_range);
		voxtra::WorldRayCollision closest = collision::raycast(cameraray, collision::HitQuery(world()));
		if (closest)
		{
			world().write_event<camera_event>(closest.unwrap().owner());
		}

	}
};
