
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
			for (auto [dynamic_tag_1, collider_1, obj_1] : colliders) {
				for (auto [dynamic_tag_2, collider_2, obj_2] : colliders) {
					if (obj_1 != obj_2) {
						if (collision::intersect_aabb(obj_1, obj_2)) {
							write_collision_event(obj_1, obj_2);
						}
					}
				}
			}
		}
	};
	struct StaticCollsionSystem :ecs::System {

		void run(ecs::Ecs& world) {
			grid::Grid& grid = world.get_resource<grid::Grid>();
			ecs::View<DynamicCollider,ecs::Constrained<Collider>,ecs::Owner> colliders(world);
			
			for (auto&& [dynamic_tag, collider, object] : colliders) {
				geo::Box entity_box = global_box(collider).expanded(v3::unit_scale/ 100.0f);
				for(v3::Coord crd:geo::IntBox3d(grid.get_voxel(entity_box.min()), grid.get_voxel(entity_box.max()))){
					stn::Option<chunks::block_object&> blk = grid.get_object(crd);
					if (!blk) {
						continue;
					}
					stn::Option<Collider&> collision = blk.unwrap().get_component_opt<Collider>();
					if (!collision) {
						continue;
					}
					if (collision::intersect_aabb(blk.unwrap().object(), collider)) {
						collision::write_collision_event(blk.unwrap().object(), object);
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