#include "aabb.h"
#include "../math/geometry.h"
//simple aabb class without collision implemented 
namespace aabb {

	//we can use a dynamic_body component as well in order to remove this 
	Sparse::PackedSet<Collider*> Colliderlist;
	void initCollider() {
		Colliderlist = Sparse::PackedSet<Collider*>();

	}
	Option<v3::Vec3> collideaabb(Collider& p1, Collider& p2) {
		return geo::collide_box(p1.globalbox(), p2.globalbox());

	}



	void Collider::destroy_hook() {
		//if index is not null which implies that ojbect is nto null
		if (in_list) {
			Colliderlist.erase(this);

		}
	}
	Collider::Collider(v3::Point3 objcenter, v3::Scale3 objscale, bool appendtolist, bool iseffector) : box(objcenter, objscale) {
		effector = iseffector;
		in_list = appendtolist;
		if (in_list) {
			Colliderlist.push(this);
		}
	}

	bool  aabbboxintersect(geo::Box p1, Collider& p2) {
		return geo::boxes_intersect(p1, p2.globalbox());
	}

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<aabb::Collider> = {
		.updates = false
	};
}
