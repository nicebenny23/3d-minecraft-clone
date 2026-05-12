#pragma once 

#include "../world/voxeltraversal.h"
#include "../game/collision.h"
#include "../game/health.h"
#include "../debugger/console.h"
#include "player_look.h"
#include "../game/particles.h"
#include "../entities/onhit.h"
namespace player {

	struct PlayerAttackParticleSpawner :renderer::ParticleSpawner {
		void apply(ecs::obj entity, v3::Point3 position) {
			math::Transform& transform = entity.add_component<core::LocalTransform>(random::spherical() + position).transform;
			transform.look_towards(random::spherical());
			transform.scale = v3::Scale3(1 / 3.0f);
			entity.apply_recipe(DynamicColliderRecipe(true));
			entity.apply_recipe(physics::Spawner{ .gravity = physics::Force{.force = v3::Vec3(0.0f,-.4f,0)} });
			entity.get_component<physics::rigidbody>().add_impluse(physics::Implulse(v3::Vec3(0, .4, 0) + random::spherical().with_y(0) * 15.0f));
			entity.get_component<physics::FrictionDamping>().strength = .2f;

			//entity.add_component<Health::DestroyOnHit>();
			entity.apply_recipe(DecalRecipe{ .path = "images\\dust.png" });
			entity.get_component<renderer::Model>().color = colors::DarkGray;
		}
	};
	struct PlayerAttack
		: ecs::component {
		PlayerAttack(ecs::Constrained<renderer::ParticleEmmitter> emit, timing::WorldClock& clock) :emmiter(emit), last_attack(clock), last_attack_click(clock), last_time_seen(clock) {

		};
		timing::Duration last_attack;
		timing::Duration last_attack_click;
		timing::Duration last_time_seen;

		voxtra::RayWorldCollision last_seen_entity;
		ecs::Constrained<renderer::ParticleEmmitter> emmiter;
	};

	struct PlayerAttacker :ecs::System {
		void run(ecs::Ecs& world) {

			ecs::View < PlayerAttack, player::PlayerCursor, player::inventory, ecs::Owner> attack_query(world);
			for (auto&& [attack, cursor, inventory, owning_object] : attack_query) {
				if (attack.last_attack.is_active()) {
					continue;
				}
				double attack_lifetime = .15f;
				if (world.get_resource<userinput::InputManager>().left_mouse().pressed) {
					attack.last_attack_click.set(attack_lifetime);
				}

				if (cursor.hit) {
					voxtra::RayWorldHit closest = cursor.hit.unwrap();
					ecs::obj object = closest.collider.object();
					if (object.has_components<Health::EntityHealth, physics::rigidbody>()) {
						if (object.get_component<Health::EntityHealth>().damage_delay_timer.is_inactive()) {
							attack.last_time_seen.set(attack_lifetime);
							attack.last_seen_entity = closest;
							//this is not the right position but close enough
						}
					}
				}
				if (attack.last_attack_click.is_active() && attack.last_seen_entity && attack.last_seen_entity.unwrap().owner().exists() && attack.last_time_seen.is_active()) {
					size_t dmg = 1;
					stn::Option<ecs::Constrained<items::item_stack>> item = inventory.selected_object();
					ecs::obj object = attack.last_seen_entity.unwrap().owner();
					if (item) {
						items::item_id id = item.unwrap().get<items::item_stack>().contained_id();
						dmg = world.get_resource<items::ItemTypes>().from_id(id)
							.tool_info().member(&items::tool_traits::damage).unwrap_or_default();
						item.unwrap().get_component_opt<items::item_durability>().then([](items::item_durability& dur) {dur.use(); });
					}
					dmg = stn::max(1, dmg);
					attack.emmiter.get<renderer::ParticleEmmitter>().add_spawns(10);
					if (object.get_component<Health::EntityHealth>().current_health <= dmg) {
						attack.emmiter.get<renderer::ParticleEmmitter>().add_spawns(30);

					}
					geo::ray ray = attack.last_seen_entity.unwrap().ray();
					attack.emmiter.get_component<core::LocalTransform>().transform.position = ray.end;
					world.write_command(Health::AttackCommand{ .knockback_multiplier = 6,.damage = dmg,.center = ray.start,.body = object });
					double attack_cooldown = 1;
					attack.last_attack.set(attack_cooldown);
					//no need to set the other ones
					if (object.has_component<items::loot_dropper>()) {
						object.get_component<items::loot_dropper>().set(owning_object);
					}
				}
			}
		}

	};
}
