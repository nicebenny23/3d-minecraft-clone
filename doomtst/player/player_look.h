#include "../game/ecs/game_object.h"
#include "../game/collision.h"
#include "../player/player.h"
#include "../math/interval.h"
#include "../game/casts.h"

#pragma once
namespace player {
	struct CursorHit {
		ecs::obj hit;
	};
	struct PlayerCursor:ecs::component {

		voxtra::WorldRayCollision Hit;
		math::range look_range= math::range(0,4);
	};

	struct PlayerCursorCaster:ecs::System{
		void run(ecs::Ecs& world) {
			ecs::View<ecs::With<ecs::transform_comp>, ecs::With<PlayerCursor>,ecs::Owner>look_view(world);
			for (stn::TupleSet<ecs::transform_comp&,PlayerCursor&,ecs::obj> view: look_view) {
				
				PlayerCursor& cursor=view.get<PlayerCursor&>();
				math::Transform& transform= view.get<ecs::transform_comp&>().transform;
				math::ray look_ray=transform.forward_ray().dialate_from_start(cursor.look_range.upper());

				cursor.Hit = collision::raycast(look_ray, collision::HitQuery(view.get<ecs::obj>()));
				if (cursor.Hit) {
					voxtra::RayWorldHit hit = cursor.Hit.unwrap();
					world.write_event(CursorHit{.hit=hit.owner() });
				}
			}
		}

	};
}