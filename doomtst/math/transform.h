#pragma once
#include "vector3.h"
#include <glm/mat4x4.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ray.h"
#include <glm/gtx/euler_angles.hpp>
#include "geometry.h"
#include "angle.h"
namespace math {
	inline glm::mat4 lookRotationMatrix(v3::Vec3 vec) {
		v3::Vec3 forward = vec;
		v3::Vec3 WorldUp = v3::up;
		if (v3::cross(forward, WorldUp) == v3::zerov) {
			WorldUp = v3::Vec3(1.0f, 0.0f, 0.0f);
		}
		v3::Vec3 right = v3::normal(v3::cross(WorldUp, forward));
		v3::Vec3 trueUp = v3::normal(v3::cross(forward, right));
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation[0] = glm::vec4(right.glm(), 0.0f);
		rotation[1] = glm::vec4(trueUp.glm(), 0.0f);
		rotation[2] = glm::vec4(forward.glm(), 0.0f);
		return rotation;
	}

	struct Transform {
		Transform() :look(), scale(v3::unit_scale) {
		}
		Transform(v3::Point3 pos, Look3 look_dir, v3::Scale3 scl):look(look_dir){
			position = pos;
			scale = scl;
		}
		v3::Point3 position;
		math::Look3 look;
		v3::Scale3 scale;
		void from_box_unrotated(geo::Box box) {
			position = box.center;
			scale = box.scale;
		}
		geo::Box unrotated_box() const{
			return geo::Box(position, scale);
		}
		void rotate(math::Look3 offset) {
			look.pitch+= offset.pitch;
			look.yaw += offset.yaw;

		}
		static double distance(const Transform& t1, const Transform& t2) {
			return v3::dist(t1.position, t2.position);
		}
		v3::Vec3 normal_dir() const{
			return v3::yaw_pitch(look);
		}
		v3::Vec3 right_dir() const {
			return v3::zero_fixed_normal(v3::cross(normal_dir(), v3::up));
		}
		geo::ray forward_ray() const {
			return geo::ray::from_offset(position, normal_dir());
		}
		v3::Vec3 up_dir() const{
			return v3::cross(right_dir(), normal_dir());
		}
		void look_towards(v3::Vec3 direction) {
			direction = v3::zero_fixed_normal(direction);
			look.pitch = Angle::from_radians(std::asin(direction.y));
			look.yaw = Angle::from_radians(std::atan2(direction.z, direction.x));
		}
		void look_at(v3::Point3 LookTowards) {
			look_towards(LookTowards - position);
		}
		glm::mat4 as_matrix() {
			glm::mat4 rotation = lookRotationMatrix(normal_dir());
			glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), position.glm());
			glm::mat4 modelMatrix = translation * rotation * scaleMat;
			return modelMatrix;
		}
	};

	inline glm::mat4 LookAt(Transform& transform) {
		return glm::lookAt(transform.position.glm(), transform.position.glm() + transform.normal_dir().glm(), transform.up_dir().glm());
	}

}