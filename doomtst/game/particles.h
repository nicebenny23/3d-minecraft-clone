#pragma once
#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include "../util/queue.h"

#include "../game/time.h"
#include "../renderer/renderer.h"

namespace renderer {
	struct ParticleSpawner {
		ParticleSpawner() {

		}
		virtual void apply(ecs::obj entity, v3::Point3 position) = 0;
		virtual ~ParticleSpawner() = default;
	};
	struct EventualDestroy:ecs::component {
		EventualDestroy(timing::Duration end):destroy_time(end) {

		}
		timing::Duration destroy_time;
	};
	struct ParticleEmmitter : ecs::component {
		bool enabled=false;
		ParticleEmmitter(stn::box<ParticleSpawner> spawn,double duration_length) :spawner(std::move(spawn)), particle_max_lifetime(duration_length){
			spawn_counter = 0;
		}
		void add_spawns(size_t counter_inc) {
			spawn_counter += counter_inc;
		}
		size_t spawn_counter;
		double particle_max_lifetime;
		stn::box<ParticleSpawner> spawner;
	};
	struct ParticleEmmisionSystem :ecs::System {
		void run(ecs::Ecs& world) {
			timing::WorldClock& time = world.get_resource<timing::WorldClock>();
			ecs::View<ParticleEmmitter, core::LocalTransform> emmisions(world);
			for (auto&& [emmitter,transform] :emmisions) {
					for (size_t i = 0; i < emmitter.spawn_counter; i++) {
						ecs::obj next = world.spawn_empty();
						next.add_component < EventualDestroy>(time.make_duration()).destroy_time.set(emmitter.particle_max_lifetime);
						emmitter.spawner->apply(next, transform.transform.position);
					}
					emmitter.spawn_counter=0;

			}
			ecs::View<EventualDestroy,ecs::Owner> particles(world);
			for (auto&& [particle, object] : particles) {
				if (particle.destroy_time.is_inactive()) {
					object.destroy();
				}
			}
		}
	};

	inline void particle_plugin(core::App& app) {
		app.emplace_system<ParticleEmmisionSystem>();
	};
	template<std::derived_from<ParticleSpawner> T>
	struct ParticleEmmitterRecipe {
		double max_lifetime;
		void apply(ecs::obj& object) const {
			object.apply_recipe<core::TransformRecipe>(v3::Point3(0, 0, 0));
			timing::Duration dur = object.world().get_resource<timing::WorldClock>().make_duration();
			object.add_component<ParticleEmmitter>(stn::box<ParticleSpawner>(stn::construct_derived<T>()), max_lifetime);
		}
	};
}