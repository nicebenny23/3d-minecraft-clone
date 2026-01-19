#pragma once
#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "../math/ray.h"
#include "../math/geometry.h"
#include "../util/random.h"
#include "../block/block.h"
#include "../util/SparseSet.h"
#include <cmath>

#include "objecthelper.h"
#include "../math/transform.h"

namespace aabb {



	struct Collider : ecs::component {
		bool effector;
		//local box
		geo::Box box;
		//world box
		geo::Box globalbox() const{



			if (owner().has_component<ecs::transform_comp>()) {
				math::Transform transform = owner().get_component<ecs::transform_comp>().transform;

				return geo::Box(box.center * transform.scale * 2 + v3::Vec3(transform.position.x, transform.position.y, transform.position.z), box.scale * transform.scale * 2);


			}
			if (owner().has_component<blockname::block>()) {
				blockname::block& blk = owner().get_component<blockname::block>();
				Scale3 scale = blk.mesh.box.scale / blockname::blockscale;
				Point3 pos = blk.center();
				return geo::Box(box.center * scale + v3::Vec3(pos.x, pos.y, pos.z), box.scale * scale);

			}
		}
		v3::Scale3 scale() const {
			return globalbox().scale;
		}
		v3::Point3 center() const {
			return globalbox().center;
		}
		bool in_list;

		Collider() = default;
		Collider(const v3::Point3& objcenter, const v3::Scale3& objscale, bool appendtolist, bool iseffector = false);
		~Collider() = default;


		void destroy_hook() override;


	};

	void initCollider();
	bool aabbboxintersect(geo::Box p1, Collider& p2);
	//cannot consify until global box is const
	Option<v3::Vec3> collideaabb(Collider& p1, Collider& p2);

	extern Sparse::PackedSet<Collider*> Colliderlist;
}

