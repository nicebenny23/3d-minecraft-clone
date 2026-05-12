
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#include "../game/health.h"
#pragma once 
namespace Health {
	struct DestroyOnHit :ecs::component {

	};
	struct DamageOnHit : ecs::component {
		float knockback;
		ecs::Constrained<Health::EntityHealth, core::LocalTransform> target;
		size_t damage;
		DamageOnHit(ecs::Constrained<Health::EntityHealth, core::LocalTransform> target,size_t dmg, float kb):target(target){
			damage = dmg;
			knockback = kb;

		}
	};
	struct HitProccesor :ecs::System {
		HitProccesor(ecs::Ecs& world) :events(world.make_reader<collision::collision_event>()) {

		}
		ecs::EventReader<collision::collision_event> events;
		void run(ecs::Ecs& world) {
			for (collision::collision_event event : events.read()) {
				if (event.source.has_components<DestroyOnHit>()) {
					event.source.destroy();
				}
				if (event.source.has_components<DamageOnHit, core::LocalTransform>()) {
					DamageOnHit& hit = event.source.get_component<DamageOnHit>();
					if (hit.target.object() == event.target) {

						v3::Point3 center = event.source.get_component<core::LocalTransform>().transform.position;
						world.write_command(Health::AttackCommand{ .knockback_multiplier = hit.knockback, .damage = hit.damage,.center = center,.body = event.target });
					}

				}
			}
		}
	};
}