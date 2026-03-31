
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#include "../game/health.h"
#pragma once 
namespace Health {
	struct DamageOnHit : ecs::component {
		float knockback;
		size_t damage;
		DamageOnHit(size_t dmg, float kb) {
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
				if (event.source.has_components<DamageOnHit, ecs::world_transform>()) {
					if (event.target.has_components<Health::EntityHealth, ecs::world_transform, physics::rigidbody>()) {
						v3::Point3 center = event.source.get_component<ecs::world_transform>().transform.position;
						DamageOnHit& hit = event.source.get_component<DamageOnHit>();
						world.write_command(Health::AttackCommand{ .knockback_multiplier = hit.knockback, .damage = hit.damage,.center = center,.body = event.target });
					}
				}
			}
		}
	};
}