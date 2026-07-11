#pragma once
#include "../block/block.h"
#include "../game/ecs/game_object.h"
#include "../math/geometry.h"
#include "../math/transform.h"
#include "../math/vector3.h"
#include "../util/Option.h"
#include "ecs/component.h"

#include "../game/transforms.h"
namespace aabb {



	struct Collider : ecs::component {
		bool effector;
		//local box
	
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

	inline	bool  box_intersects_aabb(geo::Box p1, ecs::Constrained<Collider> p2) {
		return geo::boxes_intersect(p1, global_box(p2));
	}

	//cannot consify until global box is const
	inline Option<v3::Vec3> collide_aabb(ecs::Constrained<Collider> p1, ecs::Constrained<Collider> p2) {
		//this is until i can get the effectors on the movment
		return geo::collide_box(global_box(p1).expanded(1/ 100.f), global_box(p2));

	}

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<aabb::Collider> = {
		.updates = false
	};
}


