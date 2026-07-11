#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../game/aabb.h"
#include "../game/time.h"
#include "../math/geometry.h"
#include "../world/voxeltraversal.h"
#include "ecs/query.h"
#include "../util/pipeline.h"
#include "casts.h"
#include "../game/transforms.h"
#include "ecs/query.h"
#include "../game/transforms.h"
#include "collision.h"

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
		Gravity(Force gravity_strength) :strength{ gravity_strength } {

		}

		Force strength;
	};
	struct FrictionDamping :ecs::component {
		double strength = 4.f;
		FrictionDamping(double friction_strength):strength(friction_strength){
		}
	};

	struct PhycicsMaterial:ecs::component {
		PhycicsMaterial(double bouncyness) :resitution(bouncyness) {

		}
		double resitution;
	};
	struct Density:ecs::component {
		Density(double object_density) :density(object_density) {

		}
		double density;
	};
	struct RigidBody : ecs::component {
		Vec3 velocity;
		Vec3 acceleration;
		double mass;

		bool isonground;
		void add_force(const Force& force) {
			acceleration += force.force / mass;
		}
		void add_impluse(const Implulse& impulse) {
			velocity += impulse.impulse / mass;
		}
		// Constructor
		RigidBody() :mass(1), velocity(zerov), acceleration(zerov) {
			isonground = true;
		}
		~RigidBody() = default;
	};

	struct RigidbodySystem :ecs::System {

		void run(ecs::Ecs& ecs) override {
			double deltaTime = ecs.ensure_resource<timing::WorldClock>().dt;
			ecs::View< RigidBody,Density,core::LocalTransform> densities(ecs);
			for (auto [body, density,transform] : densities) {
				body.mass=transform.transform.scale.volume()*density.density;
			}

			ecs::View< Gravity,  RigidBody> fallers(ecs);
			for (auto [grav, body] : fallers) {
				body.add_force(grav.strength);
			}
			ecs::View< FrictionDamping, RigidBody> friction_query(ecs);
			for (auto [friction, body] : friction_query) {
				body.add_force(Force{ .force = -body.velocity * friction.strength * v3::Scale3(1.0f,0.f,1.0f) });
			}

			ecs::View<core::LocalTransform,RigidBody,PhycicsMaterial,ecs::Mabye<aabb::Collider>, ecs::Owner> rigids(ecs);
			for (auto [pos, body, material, collider_mabye, object] : rigids) {
				if (collider_mabye.is_some_and([&](aabb::Collider& collider){return !collider.effector;})) {
				

					aabb::Collider& collider = collider_mabye.unwrap();
					double hit_toi = ecs.ensure_resource<timing::WorldClock>().dt;
					while (hit_toi > 0) {
						v3::Point3 curr_pos = pos.transform.position;
						v3::Point3 new_pos = curr_pos + body.velocity * hit_toi;
						geo::RayBox dir_ray(geo::ray(curr_pos, new_pos), pos.transform.scale);
						voxtra::RayWorldCollision coll = collision::ray_box_cast(dir_ray, collision::HitQuery(object));
						if (!coll) {
							pos.transform.position = curr_pos + body.velocity * hit_toi * .99f;
							hit_toi = 0;
						}
						else {
							voxtra::RayWorldHit hit = coll.unwrap();
							stn::Option<math::Direction3d> collision_dir_mabye = hit.hit_direction();
							double move_time = hit_toi * hit.hit.length() / dir_ray.ray.length();
							if (!collision_dir_mabye) {
								v3::Vec3 offset = geo::collide_box(dir_ray.start_box().expanded(.001f), global_box(hit.collider)).unwrap();
								pos.transform.position += offset;
								break;
							}
							v3::Vec3 collision_normal = collision_dir_mabye.unwrap().vec();
							hit_toi -= move_time;
							//produces many extra events but ensures each one is hit todo find a better framework
							collision::write_collision_event(object, hit.owner());
							v3::Point3 hit_pos = coll.unwrap().intersection() + collision_normal * 1e-3;
							pos.transform.position = hit_pos;
							

							//phycics materials
							v3::Vec3 vel_normal = collision_normal * v3::dot(body.velocity, collision_normal);
							double restitution = 0;
							double our_mass = body.mass;
							double their_mass = std::numeric_limits<double>().infinity();
							stn::Option<RigidBody&> other_body = hit.owner().get_component_opt<RigidBody>();
							if (other_body) {
								their_mass = other_body.unwrap().mass;
							}
							stn::Option<PhycicsMaterial&> other_material = hit.owner().get_component_opt<PhycicsMaterial>();
							if (other_material) {
								restitution= (material.resitution+ other_material.unwrap().resitution)/2;
								
							}
							Vec3 impulse = vel_normal * -(restitution+ 1) / (1 / our_mass + 1 / their_mass);
							body.add_impluse(Implulse(impulse));
							if (other_body) {
								other_body.unwrap().add_impluse(Implulse(-impulse));
							}

						}
					}

					Point3 boxcenter = pos.transform.unrotated_box().in_direction(math::down_3d);
					geo::Box checkbox = geo::Box(boxcenter, pos.transform.unrotated_box().scale.with_y(.005) * .96f);
					body.isonground = collision::boxcast(checkbox, collision::HitQuery(object));
				}
				else {
					pos.transform.position += body.velocity * deltaTime;
				}
				body.velocity += body.acceleration * deltaTime;
				body.acceleration = Vec3(0, 0, 0);
			}

		}

	};

	struct PhysicsPlugin {
		void operator()(core::App& world) {

			world.insert_plugin(timing::TimePlugin());
			world.emplace_system< RigidbodySystem>();
		}
	};

	struct Spawner {
		double restitution = .1f;
		double density = 1.0f;
		Force gravity = Force(Vec3(0, -9.8, 0));
		double friction = 4.0f;
		void apply(ecs::obj& object) const {
			object.add_component<Density>(density);
			object.add_component<RigidBody>();
			object.add_component<FrictionDamping>(friction);
			object.add_component<PhycicsMaterial>(restitution);
			object.add_component<Gravity>(gravity);
		}
	};
}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<physics::RigidBody> = {
		.priority = -111
	};
}