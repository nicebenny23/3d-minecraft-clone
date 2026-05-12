#pragma once
#include "../block/block.h"
#include "../game/ecs/game_object.h"
#include "../math/geometry.h"
#include "../math/Scale3.h"
#include "../math/transform.h"
#include "../math/vector3.h"
#include "../util/Option.h"
#include "ecs/component.h"

#include "../game/transforms.h"
namespace aabb {



	struct Collider : ecs::component {
		bool effector;
		//local box
		geo::Box global_box() const {
			if (owner().has_component<core::LocalTransform>()) {
				return owner().get_component<core::LocalTransform>().transform.unrotated_box();
			}
			if (owner().has_component<blocks::block>()) {
				return owner().get_component<blocks::block>().bounds();
			}
		}

		v3::Point3 center() const {
			return global_box().center;
		}
		Collider(bool iseffector = false) : effector(iseffector) {

		}
	};
	inline geo::Box global_box(ecs::Constrained<Collider> collider) {
		stn::Option<math::Transform> transform = collider.get_component_opt< core::LocalTransform>().member(&core::LocalTransform::transform);
		if (transform) {
			return transform.unwrap().unrotated_box();
		}
		else {
			return collider.get_component<blocks::block>().bounds();
		}
	}
	struct DynamicCollider :ecs::component {



	};

	struct DynamicColliderRecipe {
		bool effector;

		DynamicColliderRecipe(bool is_effector = false) :effector(is_effector) {

		}
		void apply(ecs::obj& object) const{
			object.add_component<DynamicCollider>();
			object.add_component<Collider>(effector);
		}
	};

	inline	bool  box_intersects_aabb(geo::Box p1, Collider& p2) {
		return geo::boxes_intersect(p1, p2.global_box());
	}

	//cannot consify until global box is const
	inline Option<v3::Vec3> collide_aabb(Collider& p1, Collider& p2) {
		//this is until i can get the effectors on the movment
		return geo::collide_box(p1.global_box().expanded(v3::Scale3() / 100.f), p2.global_box());

	}

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<aabb::Collider> = {
		.updates = false
	};
}


