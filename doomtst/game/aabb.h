#pragma once
#include "../block/block.h"
#include "../game/ecs/game_object.h"
#include "../math/geometry.h"
#include "../math/transform.h"
#include "../math/vector3.h"
#include "../util/Option.h"
#include "ecs/component.h"

#include "../game/transforms.h"
#include "../math/intersection.h"
namespace collision {



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
	inline bool intersect_aabb(ecs::Constrained<Collider> p1, ecs::Constrained<Collider> p2) {
		//this is until i can get the effectors on the movment
		return geo::boxes_intersect(global_box(p1).expanded(1/ 100.f), global_box(p2));

	}


	struct RayWorldHit {
		ecs::Constrained<Collider> collider;
		geo::RayHit hit;

		RayWorldHit(geo::RayHit rayHit, ecs::Constrained<Collider> WorldCollider) :hit(rayHit), collider(WorldCollider) {
		}
		Point3 intersection() const {
			return ray().end;
		}

		ecs::obj owner() const {
			return collider.object();
		}

		double dist() const {
			return ray().length();
		}
		stn::Option<math::Direction3d> hit_direction() const {
			return hit.hit_normal;
		}

		geo::ray ray() const {
			return hit.ray;
		}
	};
	using RayWorldCollision = stn::Option<RayWorldHit>;



	struct HitQuery {
		ecs::obj orgin;
		explicit HitQuery(const ecs::obj& orgin_obj) : orgin(orgin_obj) {
		}

		bool operator()(const ecs::Constrained<collision::Collider>& collider) const {
			return collider.object() != orgin && !collider.get<collision::Collider>().effector;
		}
	};


	struct SolidPredicate {
		inline bool operator()(const ecs::Constrained<Collider>& block) const {
			if (block.get_component<Collider>().effector) {
				return false;
			}
			return true;
		}
	};

	template<typename T>
	concept ObjectPredicate = std::predicate<T, const ecs::Constrained<Collider>& >;
	//casting

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<collision::Collider> = {
		.updates = false
	};
}


