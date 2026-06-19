

#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/health.h"
#include "../items/menu.h"
#include "../math/mathutil.h"
#pragma once
struct playercamcontrols : ecs::component
{
	void update() {


		if (!world().ensure_resource<ui::MenuState>().menu_open())
		{
			renderer::Window& window=world().get_resource<renderer::Window>();
			window.disable_cursor();

			userinput::InputManager& man = world().get_resource<userinput::InputManager>();
			double xoffset = man.adjusted_mouse_position_dt.x;
			double yoffset = man.adjusted_mouse_position_dt.y;

			float sensitivity = 60;

			if (math::abs(xoffset) > 180)
			{
				xoffset = 0;
			}  if (math::abs(yoffset) > 180)
			{
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity* window.aspect_ratio();

			owner().get_component<core::LocalTransform>().transform.rotate(math::Look3::from_degrees(xoffset, yoffset));

			owner().get_component<core::LocalTransform>().transform.look.pitch = math::Angle::from_degrees(clamp(owner().get_component<core::LocalTransform>().transform.look.pitch.degrees_signed(), -89.99, 89.99));

		}
		else
		{
			Core::game.Ecs.get_resource<renderer::Window>().enable_cursor();
		}
		
	}
};
