#pragma once
#include "vector3.h"
#include <glm/mat4x4.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/rotate_vector.hpp> 
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "ray.h"
#include <glm/gtx/euler_angles.hpp>
namespace math {
	inline glm::mat4 lookRotationMatrix(v3::Vec3 vec) {
		v3::Vec3 forward = vec;
		v3::Vec3 WorldUp = v3::up;
		if (v3::Cross(forward, WorldUp) == v3::zerov) {
			WorldUp = v3::Vec3(1.0f, 0.0f, 0.0f);
		}
		v3::Vec3 right = v3::normal(v3::Cross(WorldUp, forward));
		v3::Vec3 trueUp = v3::normal(v3::Cross(forward, right));
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation[0] = glm::vec4(right.glm(), 0.0f);
		rotation[1] = glm::vec4(trueUp.glm(), 0.0f);
		rotation[2] = glm::vec4(forward.glm(), 0.0f);
		return rotation;
	}

	struct Transform {
		Transform() :yaw(90), pitch(0), scale(v3::unit_scale) {
		}
		Transform(v3::Point3 pos, double newyaw, double newpitch, v3::Scale3 newscale) {
			position = pos;
			yaw = newyaw;
			pitch = newpitch;
			scale = newscale;
		}
		v3::Point3 position;
		double yaw;
		double pitch;
		v3::Scale3 scale;

		void rotate(double ptch, double yw) {
			pitch += ptch;
			yaw += yw;
		}
		v3::Vec3 normal_dir() const{
			return v3::yaw_pitch(yaw, pitch);
		}
		v3::Vec3 right_dir() const {
			return v3::zero_fixed_normal(v3::Cross(normal_dir(), v3::up));
		}
		ray forward_ray() const {
			return ray::from_offset(position, normal_dir());
		}
		v3::Vec3 up_dir() const{
			return v3::Cross(right_dir(), normal_dir());
		}
		void look_towards(v3::Vec3 Direction) {
			Direction = v3::zero_fixed_normal(Direction);
			pitch = glm::degrees(std::asin(Direction.y));
			yaw = glm::degrees(std::atan2(Direction.z, Direction.x));
		}
		void look_at(v3::Point3 LookTowards) {
			look_towards(LookTowards - position);
		}
		glm::mat4 ToMatrix() {
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

	inline Transform Decompose(const glm::mat4& matrix) {
		glm::vec3 scale, pos, skew;
		glm::quat quatRotation;
		glm::vec4 perspective;
		bool success = glm::decompose(matrix, scale, quatRotation, pos, skew, perspective);
		if (!success) {
			throw std::logic_error("decomposition did not succeed");
		}
		glm::vec3 eulerRotation = glm::eulerAngles(quatRotation);
		Transform transform;
		transform.position = v3::Point3(pos.x, pos.y, pos.z);
		transform.yaw = glm::degrees(eulerRotation.y);
		transform.pitch = glm::degrees(eulerRotation.x);
		transform.scale = v3::Scale3(scale.x, scale.y, scale.z);
		return transform;
	}

	inline Transform Compose(Transform& t1, Transform& t2) {
		glm::mat4 mat1 = t1.ToMatrix();
		glm::mat4 mat2 = t2.ToMatrix();
		glm::mat4 composed = mat2 * mat1;
		return Decompose(composed);
	}
}