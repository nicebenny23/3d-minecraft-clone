#include "rigidbody.h"
#include "../renderer/ModelMesh.h"
#pragma once 
namespace Health {

	struct entityeffects {
		float timetilllavaover;
	};
	struct EntityHealth :ecs::component {
		EntityHealth(size_t max, size_t current, timename::Duration duration) :max_health(max), current_health(current), damage_delay_timer(duration) {

		}
		size_t max_health;
		size_t current_health;
		timename::Duration damage_delay_timer;
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
					current_entity_health.damage_delay_timer.set(4.0f);
					current_entity_health.current_health -= stn::min(current_entity_health.current_health, cmd.damage);
				}
			}
			ecs::View<ecs::With< EntityHealth>, ecs::Owner> objects(world);
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
			object.add_component<EntityHealth>(spawn_health, spawn_health, object.world().get_resource<timename::TimeManager>().current_time());
		}
	};

	struct AttackCommand {

		size_t damage;
		v3::Point3 center;
		ecs::Constrained<ecs::world_transform, physics::rigidbody,EntityHealth> body;
	};
	struct KbSystem :ecs::System {
		void run(ecs::Ecs& world) {
			for (AttackCommand& kb : world.read_commands< AttackCommand>()) {
				EntityHealth& health = kb.body.get<EntityHealth>();
				if (health.damage_delay_timer.is_active()) {

					Vec3 forceval = kb.body.get<ecs::world_transform>().transform.position - kb.center;
					forceval = Vec3(forceval.x, .25, forceval.z);
					forceval = forceval.with_magnitude(kb.damage);
					kb.body.get<physics::rigidbody>().add_impluse(physics::Implulse{ forceval });
					world.write_command(DamageCommand{ .damage = kb.damage,.target = kb.body.reduce() });
				}

			}
		}
	};
	struct FallDamageRecipient:ecs::component{
		stn::Option<double> fall_velocity;
		double damage_magnitude;
	};

	struct FallDamageSystem:ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<ecs::With<physics::rigidbody>,ecs::With< physics::Gravity>, ecs::With<FallDamageRecipient>,ecs::With<EntityHealth>,ecs::Owner> query(world);
			for (auto&& [rigid,gravity,faller,health,object]: query) {
				if (rigid.isonground) {
					if (faller.fall_velocity) {
						int dmg(faller.damage_magnitude * faller.fall_velocity.unwrap());
						if (0 < dmg) {
							world.write_command(DamageCommand{ .damage = size_t(dmg),.target = object});;
						}
					}
					faller.fall_velocity = v3::dot(rigid.velocity, gravity.strength.force);
				}
				else {
					faller.fall_velocity = stn::None;
				}
			}
		}
	};
	struct FlashOnHit:ecs::component{
		colors::Color flash_color = colors::Red;

	};
	struct DamageDisplaySystem :ecs::System {
		void run(ecs::Ecs& world) {
			ecs::View<ecs::With<Model>,ecs::With<EntityHealth>,ecs::With<FlashOnHit>> query(world);
			for (auto&& [model,health,flash]:query) {
				if (health.damage_delay_timer.is_active()) {
					model.color = flash.flash_color;
				}
				else {
					model.color = colors::White;
				}
			}
		}
	};
	inline void entity_health_plugin(Core::App& app) {
		app.emplace_system<KbSystem>();
		app.emplace_system<EntityKiller>();
		app.emplace_system<FallDamageSystem>();
		app.emplace_system<DamageDisplaySystem>();
	}
}