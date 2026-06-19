#include "rigidbody.h"
#include "../renderer/ModelMesh.h"
#pragma once 
namespace Health {

	struct entityeffects {
		float timetilllavaover;
	};
	struct EntityHealth :ecs::component {
		EntityHealth(size_t max, size_t current, timing::Duration duration) :max_health(max), current_health(current), damage_delay_timer(duration) {

		}
		size_t max_health;
		size_t current_health;
		timing::Duration damage_delay_timer;
	};

	struct DamageCommand {
		size_t damage;
		ecs::Constrained<EntityHealth> target;
	};
	
	struct EntityKiller :ecs::System {
		void run(ecs::Ecs& world) {
			for (DamageCommand& cmd : world.read_commands<DamageCommand>()) {
				EntityHealth& current_entity_health = cmd.target.get<EntityHealth>();
				if (!current_entity_health.damage_delay_timer.is_active()) {
					current_entity_health.damage_delay_timer.set(.5f);
					current_entity_health.current_health -= stn::min(current_entity_health.current_health, cmd.damage);
				}
			}
			//not flexable enough to exclude the player yet
			ecs::View< EntityHealth, ecs::Owner,ecs::Not<player::player_tag>> objects(world);
			for (auto&& [health, object] : objects) {
				if (health.current_health == 0) {
					object.destroy();
				}
			}
		}
	};

	struct HealthSpawner {
		size_t spawn_health;
		HealthSpawner(size_t entity_health) :spawn_health(entity_health) {

		}
		void apply(ecs::obj& object) const {
			object.add_component<EntityHealth>(spawn_health, spawn_health, object.world().get_resource<timing::WorldClock>().make_duration());
		}
	};

	struct AttackCommand {
		double knockback_multiplier;
		size_t damage;
		v3::Point3 center;
		ecs::Constrained<core::LocalTransform, physics::RigidBody,EntityHealth> body;
	};
	struct KbSystem :ecs::System {
		void run(ecs::Ecs& world) {
			for (AttackCommand& kb : world.read_commands< AttackCommand>()) {
				EntityHealth& health = kb.body.get<EntityHealth>();
				if (health.damage_delay_timer.is_inactive()) {

					physics::RigidBody& body= kb.body.get<physics::RigidBody>();
					double power = kb.knockback_multiplier;
					Vec3 forceval = kb.body.get<core::LocalTransform>().transform.position - kb.center;
					
					body.velocity -= v3::project(forceval, body.velocity)/2;

					if (mag2(forceval)!=0) {
						forceval = forceval.with_y(0).with_magnitude(power);
						if (body.isonground) {
							forceval.y = 1.0f;
						}
					}
					body.add_impluse(physics::Implulse{ forceval });
					world.write_command(DamageCommand{ .damage = kb.damage,.target = kb.body.reduce() });
				}

			}
		}
	};
	struct FallDamageRecipient:ecs::component{
		FallDamageRecipient(double mag) :damage_magnitude(mag) {

		}
		stn::Option<double> fall_velocity;
		double damage_magnitude;
	};

	struct FallDamageSystem:ecs::System {

		void run(ecs::Ecs& world) {
			return;
			ecs::View< physics::RigidBody, physics::Gravity, FallDamageRecipient, EntityHealth,ecs::Owner> query(world);
			for (auto&& [rigid,gravity,faller,health,object]: query) {
				if (rigid.isonground) {
					if (faller.fall_velocity) {
						int dmg(faller.damage_magnitude * faller.fall_velocity.unwrap());
						if (1 < dmg) {
							world.write_command(DamageCommand{ .damage = size_t(dmg),.target = object});;
						}
					}
				}

				faller.fall_velocity = v3::dot(rigid.velocity, gravity.strength.force.normal());
			}
		}
	};
	struct FlashOnHit:ecs::component{
		colors::Color flash_color = colors::Color(1,.5f,.5f,1);
		bool on;
	};
	struct DamageDisplaySystem :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View< Model,EntityHealth,FlashOnHit> query(world);
			for (auto&& [model,health,flash]:query) {
				if (health.damage_delay_timer.is_active()) {
					if (!flash.on) {
						model.color = model.color * flash.flash_color;
						flash.on = true;
					}
				}
				else {
					if (flash.on) {
						model.color /= flash.flash_color;
						flash.on = false;
					}
				}
			}
		}
	};
	struct EntityHealthPlugin{
		void operator()(Core::App& app) {
			app.emplace_system<KbSystem>();
			app.emplace_system<EntityKiller>();
			app.emplace_system<FallDamageSystem>();
			app.emplace_system<DamageDisplaySystem>();
		}
	};
}