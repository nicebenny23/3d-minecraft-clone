#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../game/aabb.h"
#include "../game/time.h"
#include "../math/geometry.h"
#include "../world/voxeltraversal.h"
#include "ecs/query.h"
#include "../util/pipeline.h"
#include "../util/tag.h"
#include "casts.h"
#include "ecs/query.h"
using namespace v3;
#pragma once 
namespace physics {
	struct Implulse {
		Vec3 impulse;
	};
	struct Force {
		Vec3 force;
	};
	struct Gravity :ecs::component {
		Gravity() :strength{ .force{0,-9.8f,0} } {

		}

		Force strength;
	};
	struct FrictionDamping :ecs::component {
		double strength = 4.f;
	};

	struct rigidbody : ecs::component {
		Vec3 velocity;
		Vec3 acceleration;
		float mass = 1.0f;
		bool isonground;
		bool isonceil;
		bool inliquid;
		void add_force(const Force& force) {
			acceleration += force.force / mass;
		}
		void add_impluse(const Implulse& impulse) {
			velocity += impulse.impulse / mass;
		}
		void calculateonground() {

			inliquid = false;
			Point3 boxcenter = owner().get_component<aabb::Collider>().global_box().in_direction(math::down_3d);
			math::Box checkbox = math::Box(boxcenter, owner().get_component<aabb::Collider>().global_box().scale.with_y(.005) * .96f);
			isonground = voxtra::boxcast_grid(checkbox, world().get_resource<grid::Grid>());
		}
		bool calculateonceil() {

			Point3 boxcenter = owner().get_component<aabb::Collider>().global_box().in_direction(math::up_3d);
			math::Box checkbox = math::Box(boxcenter, owner().get_component<aabb::Collider>().global_box().scale.with_y(.005) * .96f);
			isonceil = voxtra::boxcast_grid(checkbox, world().get_resource<grid::Grid>());
			return isonceil;
		}
		// Constructor
		rigidbody() : velocity(zerov), acceleration(zerov) {

		}
		~rigidbody() = default;
	};
	struct RigidbodySystem :ecs::System {

		void run(ecs::Ecs& ecs) override {
			double deltaTime = ecs.ensure_resource<timename::TimeManager>().dt;

			ecs::View<ecs::With< Gravity>, ecs::With<rigidbody>> fallers(ecs);
			for (auto [grav, body] : fallers) {
				body.add_force(grav.strength);
			}
			ecs::View<ecs::With< FrictionDamping>, ecs::With<rigidbody>> friction_query(ecs);
			for (auto [friction, body] : friction_query) {
				body.add_force(Force{ .force = -body.velocity * friction.strength * v3::Scale3(1.0f,.2f,1.0f) });
			}

			ecs::View<ecs::With< ecs::world_transform>, ecs::With<rigidbody>, ecs::With<aabb::Collider>, ecs::Owner> rigids(ecs);
			for (auto [pos, body, collider, object] : rigids) {

				body.calculateonground();

				if (body.calculateonceil()) {

				}
				double deltaTime = ecs.ensure_resource<timename::TimeManager>().dt;


				size_t mini_steps = 5;
				for (int j = 0; j < mini_steps; j++) {


					for (int i = 0; i < 3; i++) {
						v3::Point3 curr_pos = pos.transform.position;
						v3::Point3 new_pos = curr_pos + v3::Vec3(body.velocity[i] / mini_steps, i) * deltaTime;
						int sgn = sign(body.velocity[i]);
						double scale_in_dir = collider.global_box().half_size()[i];
						v3::Vec3 max_dir_rel = Vec3(sgn * scale_in_dir, i);
						math::ray dir_ray(curr_pos + max_dir_rel, max_dir_rel + new_pos);
						if (dir_ray.length() == 0) {
							continue;
						}
						voxtra::WorldRayCollision coll = collision::raycast(dir_ray, collision::HitQuery(object));
						if (!coll) {
							pos.transform.position[i] = new_pos[i];
						}
						else {

							v3::Point3 new_posdiff = coll.unwrap().intersection() - max_dir_rel + v3::Vec3(1e-4 * (-sgn), i);
							v3::Vec3 fall_difrence = coll.unwrap().ray().diff();
							if (fall_difrence.mag2() > 1) {
								int l = 4;
							}
							body.add_force(Force{ .force = -fall_difrence * .4f });
							pos.transform.position = new_posdiff;

						}
					}
				}
				body.velocity += body.acceleration * deltaTime;
				body.acceleration = Vec3(0, 0, 0);
			}

		}

	};
	inline void PhysicsPlugin(Core::App& world) {
		world.emplace_system< RigidbodySystem>();
	}
	inline void Spawner(ecs::obj& object) {
		object.add_component<rigidbody>();
		object.add_component<FrictionDamping>();
		object.add_component<Gravity>();
	}
}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<physics::rigidbody> = {
		.priority = -111
	};
}