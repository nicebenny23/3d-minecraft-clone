#include "../game/ecs/filtered_object.h"
#include "../block/soil.h"
#include "../player/player_look.h"
#pragma once
namespace player {
	struct PlayerFertilizeParticleSpawner :renderer::ParticleSpawner {
		void apply(ecs::obj entity, v3::Point3 position) {
			math::Transform& transform = entity.add_component<core::LocalTransform>(random::spherical()*2 + position).transform;
			transform.look_towards(random::spherical());
			transform.scale = v3::Scale3(1 / 3.0f);
			entity.apply_recipe(DynamicColliderRecipe(true));
			entity.apply_recipe(physics::Spawner{ .gravity = physics::Force{.force = v3::Vec3(0.0f,-.4f,0)} });
			entity.get_component<physics::RigidBody>().add_impluse(physics::Implulse(v3::Vec3(0, .4, 0) + random::spherical().with_y(0) * 15.0f));
			entity.get_component<physics::FrictionDamping>().strength = .2f;

			//entity.add_component<Health::DestroyOnHit>();
			entity.apply_recipe(DecalRecipe{ .path = "images\\dust.png" });
			entity.get_component<renderer::Model>().color = colors::lerp(colors::DarkGray,colors::Green,.5);
		}
	};
	struct PlayerFertilizer:ecs::component{
		ecs::Constrained<renderer::ParticleEmmitter> emitter;
		PlayerFertilizer(ecs::Constrained<renderer::ParticleEmmitter> emit):emitter(emit){

		}

	};
	struct FertilizationSystem :ecs::System{
		void run(ecs::Ecs& world) {
			for (auto [fertilizer,look, inventory] : ecs::View< PlayerFertilizer, player::PlayerCursor, player::inventory>(world)) {
				if (!world.get_resource<userinput::InputManager>().right_mouse().pressed) {
					continue;
				}

				if (!look.hit) {
					continue;
				}
				voxtra::RayWorldHit hit = look.hit.unwrap();
				if (!hit.owner().has_component<blocks::Seedability>()) {
					continue;
				}
				blocks::Seedability& lt = hit.owner().get_component<blocks::Seedability>();
				
				size_t fertile = inventory.selected().map(&items::item_stack::traits).member(&items::item_traits::fertilizer).unwrap_or(0);
				if (fertile) {
					items::item_stack& stack = inventory.selected().unwrap();
					if (stack.can_remove(1)) {
						fertilizer.emitter.get_component<core::LocalTransform>().transform.position = look.hit.unwrap().intersection();
						fertilizer.emitter.get<renderer::ParticleEmmitter>().add_spawns(10 );
						lt.seedable += fertile;
						inventory.selected().unwrap().remove(1);
					}
				}
			}
		}
	};


}