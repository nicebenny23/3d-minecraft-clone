#pragma once
#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "../math/geometry.h"
#include "../block/block.h"
#include "../util/SparseSet.h"
#include "../math/transform.h"

namespace aabb {



	struct Collider : ecs::component {
		bool effector;
		//local box
		geo::Box box;
		//world box
		geo::Box global_box() const {
			if (owner().has_component<ecs::transform_comp>()) {
				math::Transform transform = owner().get_component<ecs::transform_comp>().transform;
				return geo::Box(box.center * transform.scale * 2 + v3::Vec3(transform.position.x, transform.position.y, transform.position.z), box.scale * transform.scale * 2);
			}
			if (owner().has_component<blocks::block>()) {
				blocks::block& blk = owner().get_component<blocks::block>();
				return blk.mesh.box.transform(box);
			}
		}
		v3::Scale3 scale() const {
			return global_box().scale;
		}
		v3::Point3 center() const {
			return global_box().center;
		}
		Collider(geo::Box aabb, bool iseffector = false):box(aabb),effector(iseffector){

		}
	};
	struct DynamicCollider:ecs::component {



	};

	bool  box_intersects_aabb(geo::Box p1, Collider& p2) {
		return geo::boxes_intersect(p1, p2.global_box());
	}

	//cannot consify until global box is const
	Option<v3::Vec3> collide_aabb(Collider& p1, Collider& p2) {
		return geo::collide_box(p1.global_box(), p2.global_box());

	}

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<aabb::Collider> = {
		.updates = false
	};
}


