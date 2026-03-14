
#include "../game/ecs/game_object.h"
#include "../math/mathutil.h"
#include "../math/transform.h"
#include "../math/interval.h"
#include "../math/vector3.h"
#pragma once
namespace renderer {
	inline double wrap_angle(double a) {
		return wrap_to_range(a, 0, 360);
	}
	struct CameraComponent : ecs::component {
		CameraComponent() :CamTransform(Point3(0, 0, 0), 0, 0, unit_scale),viewport(.23f, 100),fov(90){

			//utype = ecs::updaterender;
		}
		v3::Point3 center()const {
			return CamTransform.position;
		}
		double fov;
		math::bounds viewport;
		math::Transform CamTransform;
		void start() {
	
			CamTransform = owner().get_component<ecs::transform_comp>().transform;

		}
		void update() {
			owner().get_component<ecs::transform_comp>().transform.yaw = wrap_angle(owner().get_component<ecs::transform_comp>().transform.yaw);

			owner().get_component<ecs::transform_comp>().transform.pitch = wrap_to_range(owner().get_component<ecs::transform_comp>().transform.pitch, -90, 90);
			CamTransform = owner().get_component<ecs::transform_comp>().transform;

		}
	};
	struct camera_resource :ecs::resource {
		camera_resource(ecs::Constrained<CameraComponent> camera_object) :camera(camera_object) {

		}
		ecs::Constrained<CameraComponent> camera;
	};
	
}