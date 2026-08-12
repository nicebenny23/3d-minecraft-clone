
#include "../world/grid.h"
#include "../world/voxeltraversal.h"
#include "../util/Option.h"
#include "casts.h"
#include "Core.h"
using namespace collision;
#pragma once 
namespace collision {
	struct collision_event {
		//the target and source are abstrctions for dealing with events and for now each event wil lalso have a 
		ecs::obj target;
		ecs::obj source;

		collision_event(ecs::obj o1, ecs::obj o2) :source(o1), target(o2) {
		}
	};
	inline void write_collision_event(ecs::obj o1, ecs::obj o2) {
		o1.world().emplace_event<collision_event>(o1, o2);
		o2.world().emplace_event<collision_event>(o2, o1);
	}



	struct DynamicCollisionSystem :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::View< DynamicCollider,Collider, ecs::Owner> colliders(world);
			for (auto&& [dynamic_tag_1, collider_1, obj_1] : colliders) {
				for (auto&& [dynamic_tag_2, collider_2, obj_2] : colliders) {
					if (obj_1 != obj_2) {
						if (collider_2.effector&&collider_1.effector) {
							continue;
						}
						Option<v3::Vec3> force = collision::collide_aabb(obj_1,obj_2);
						if (force.is_none()) {
							continue;
						}
						write_collision_event(obj_1, obj_2);
						if (collider_1.effector || collider_2.effector) {
							continue;
						}
					}
				}
			}
		}
	};
	struct StaticCollsionSystem :ecs::System {

		void run(ecs::Ecs& world) {
			ecs::View<DynamicCollider,ecs::Constrained<Collider>,ecs::Owner> colliders(world);
			for (auto&& [dynamic_tag, collider, object] : colliders) {
				geo::Box entity_box = global_box(collider).expanded(v3::unit_scale/ 100.0f);
				array<chunks::block_object> blocks = collider.world().get_resource<grid::Grid>().voxel_in_range(entity_box);
				for (chunks::block_object& block : blocks) {
					stn::Option<Collider&> collision = block.get_component_opt<Collider>();
					if (!collision) {
						continue;
					}
					Option<Vec3> force = collision::collide_aabb(block.object(), collider);
					if (!force) {
						collision::write_collision_event(block.object(), object);
					}
				}
			}
		}
	};

	struct CollsionPlugin {
		void operator()(core::App& app) {
			app.emplace_system< StaticCollsionSystem>();
			app.emplace_system<DynamicCollisionSystem>();

		}
	};


}