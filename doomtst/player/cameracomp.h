
#include "../game/ecs/game_object.h"
#include "../math/mathutil.h"
#include "../math/transform.h"
#include "../math/interval.h"
#include "../math/vector3.h"


#include "../math/angle.h"
#include "../game/transforms.h"
#include "../game/ecs/weak_object.h"
#include "../renderer/Window.h"
#pragma once
namespace renderer {
	inline double wrap_angle(double a) {
		return math::wrap_to_range(a, 0, 360);
	}
	struct CameraComponent : ecs::component {
		CameraComponent() :viewport(.03f, 100),fov(math::Angle::from_degrees(90)){

			//utype = ecs::updaterender;
		}
	
		math::Angle fov;
		math::bounds viewport;
		
	};
	struct CameraDirectFollower :ecs::component {
		CameraDirectFollower(ecs::obj follow) :follow(follow) {

		}
		ecs::WeakObject follow;
	
	};
	struct CameraFollowerSystem:ecs::System{
		void run(ecs::Ecs& world) {
			ecs::View < CameraDirectFollower, core::LocalTransform> cameras(world);
			for (auto&& [direct_camera,transform]:cameras ) {
				stn::Option<ecs::obj> follow = direct_camera.follow.get();
				if (follow) {
					transform.transform = follow.unwrap().get_component<core::LocalTransform>().transform;
				}
			}
		}

	};
	inline geo::Frustum make_frustum(const math::Transform& t, math::Angle fov, double aspect, math::bounds frustum_bounds) {

		Point3 pos = t.position;
		Vec3 forward = t.normal_dir();
		Vec3 right_dir = t.right_dir();
		Vec3 up = t.up_dir();

		double halfV = fov.radians()* 0.5;
		double halfH = atan(tan(halfV) * aspect);

		// side planes
		geo::HalfSpace left = geo::HalfSpace(forward - right_dir * tan(halfH), pos); // left
		geo::HalfSpace right = geo::HalfSpace(forward + right_dir * tan(halfH), pos); // 
		geo::HalfSpace top = geo::HalfSpace(forward + up * tan(halfV), pos);    // top
		geo::HalfSpace down = geo::HalfSpace(forward - up * tan(halfV), pos);    // bottom

		// near / far
		geo::HalfSpace near_plane = geo::HalfSpace(forward, pos + forward * frustum_bounds.min());
		geo::HalfSpace far_plane = geo::HalfSpace(-forward, pos + forward * frustum_bounds.max());

		return geo::Frustum{ .planes{left,right,top,down,near_plane,far_plane} };
	}
	struct CameraResource :ecs::resource {
		CameraResource(ecs::Constrained<CameraComponent,core::LocalTransform> camera_object) :camera(camera_object) {

		}
	
		CameraComponent& world_camera() {
			return camera.get<CameraComponent>();
		}
		const CameraComponent& world_camera() const{
			return camera.get<CameraComponent>();
		}
		
		math::Transform& camera_transform() {
			return camera.get<core::LocalTransform>().transform;
		}
		const math::Transform& camera_transform() const {
			return camera.get<core::LocalTransform>().transform;
		}
		v3::Point3 center() const {
			return camera_transform().position;
		}
		ecs::Constrained<CameraComponent, core::LocalTransform> camera;
	};

	inline geo::Frustum world_camera_frustum(ecs::Ecs& world) {
		CameraResource& cam = world.get_resource<CameraResource>();
		CameraComponent& comp = cam.world_camera();
		renderer::Window& window = world.get_resource<renderer::Window>();
		return make_frustum(cam.camera_transform(), comp.fov, window.aspect_ratio(), comp.viewport);

	}
	struct CameraSpawner {
		void apply(ecs::obj& object) const {
			object.add_component<core::LocalTransform>(v3::Point3(0,0,0));
			object.add_component<CameraComponent>();
			object.world().insert_resource<CameraResource>(object).camera = object;
		}
	};
	
}