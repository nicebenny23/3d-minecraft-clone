

#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/health.h"
#include "../items/menu.h"
#include "../math/mathutil.h"
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

			world().get_resource<renderer::Window>().disable_cursor();

			userinput::InputManager& man = world().get_resource<userinput::InputManager>();
			double xoffset = man.mouse_position_dt.x;
			double yoffset = man.mouse_position_dt.y;

			float sensitivity = 60;

			if (math::abs(xoffset) > 180)
			{
				xoffset = 0;
			}  if (math::abs(yoffset) > 180)
			{
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity;

			owner().get_component<core::LocalTransform>().transform.rotate(math::Look3::from_degrees(xoffset, yoffset));

			owner().get_component<core::LocalTransform>().transform.look.pitch = math::Angle::from_degrees(clamp(owner().get_component<core::LocalTransform>().transform.look.pitch.degrees_signed(), -89.99, 89.99));

		}
		else
		{
			Core::game.Ecs.get_resource<renderer::Window>().enable_cursor();
		}
		double max_interact_range = 5;
		geo::ray cameraray = owner().get_component<core::LocalTransform>().transform.forward_ray().with_length(max_interact_range);
		voxtra::RayWorldCollision closest = collision::raycast(cameraray, collision::HitQuery(world()));
		if (closest)
		{
			world().write_event<camera_event>(closest.unwrap().owner());
		}

	}
};
