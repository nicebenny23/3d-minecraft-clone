

#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/health.h"
#include "../items/menu.h"
#include "../math/mathutil.h"
#pragma once
struct CameraController: ecs::component
{
};

struct CameraControlSystem:ecs::System{

	void run(ecs::Ecs& world) {

		renderer::Window& window = world.get_resource<renderer::Window>();
		if (world.get_resource<ui::MenuState>().menu_open()) {

			window.enable_cursor();
			return;
		}
		userinput::InputManager& man = world.get_resource<userinput::InputManager>();
		window.disable_cursor();
		ecs::View<ecs::Has<CameraController>, core::LocalTransform> view(world);
		for (auto [transform]:view ) {
		

			double xoffset = man.adjusted_mouse_position_dt.x;
			double yoffset = man.adjusted_mouse_position_dt.y;

			float sensitivity = 60;

			if (math::abs(xoffset) > 180) {
				xoffset = 0;
			}  if (math::abs(yoffset) > 180) {
				yoffset = 0;
			}
			xoffset *= sensitivity;
			yoffset *= sensitivity * window.aspect_ratio();

			transform.transform.rotate(math::Look3::from_degrees(xoffset, yoffset));

			transform.transform.look.pitch = math::Angle::from_degrees(math::bounds(-89.99,89.99).clamp(transform.transform.look.pitch.degrees_signed()));

		}
	}

};