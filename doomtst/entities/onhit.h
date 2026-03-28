
#include "../game/collision.h"
#include "../player/playerhealth.h"
#include "../game/rigidbody.h"
#include "../game/entityutil.h"
#pragma once 
template<typename T, bool is_not_tag>
struct destroyonhit : ecs::component {
	destroyonhit() {
	}
	void oncollision(ecs::obj collidedwith) {
		if (collidedwith.has_component<T>() ^ is_not_tag) {
			owner().destroy();
		}

	}

	~destroyonhit() = default;
};

struct DamageOnHit : ecs::component {
	float knockback;
	size_t damage;
	DamageOnHit(size_t dmg, float kb = 0) {
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
					v3::Point3 center = event.target.get_component<ecs::world_transform>().transform.position;
					v3::Point3 othercenter = event.source.get_component<ecs::world_transform>().transform.position;
					DamageOnHit& hit = event.source.get_component<DamageOnHit>();
					world.write_command(Health::AttackCommand{ .damage = hit.damage,.center = center,.body=event.target});
				}
			}
		}
	}
};