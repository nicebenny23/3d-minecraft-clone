#include "../math/vector3.h"
#include "../game/ecs/game_object.h"
#include "../game/aabb.h"
#include "../game/time.h"
#include "../math/geometry.h"
#include "../world/voxeltraversal.h"
#include "ecs/query.h"
#include "../util/pipeline.h"
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

		bool inliquid;
		void add_force(const Force& force) {
			acceleration += force.force / mass;
		}
		void add_impluse(const Implulse& impulse) {
			velocity += impulse.impulse / mass;
		}
		// Constructor
		rigidbody() : velocity(zerov), acceleration(zerov),inliquid(false) {
			isonground = true;
		}
		~rigidbody() = default;
	};
	struct RigidbodySystem :ecs::System {

		void run(ecs::Ecs& ecs) override {
			double deltaTime = ecs.ensure_resource<timename::TimeManager>().dt;

			ecs::View< Gravity,  rigidbody> fallers(ecs);
			for (auto [grav, body] : fallers) {
				body.add_force(grav.strength);
			}
			ecs::View< FrictionDamping, rigidbody> friction_query(ecs);
			for (auto [friction, body] : friction_query) {
				body.add_force(Force{ .force = -body.velocity * friction.strength * v3::Scale3(1.0f,.2f,1.0f) });
			}

			ecs::View<ecs::world_transform,rigidbody,aabb::Collider, ecs::Owner> rigids(ecs);
			for (auto [pos, body, collider, object] : rigids) {

				Point3 boxcenter = collider.global_box().in_direction(math::down_3d);
				geo::Box checkbox = geo::Box(boxcenter, collider.global_box().scale.with_y(.005) * .96f);
				body.isonground = voxtra::boxcast_grid(checkbox, ecs.get_resource<grid::Grid>());

				double hit_toi = ecs.ensure_resource<timename::TimeManager>().dt;
				while (hit_toi>0) {
						v3::Point3 curr_pos = pos.transform.position;
						v3::Point3 new_pos = curr_pos + body.velocity*hit_toi;
						geo::RayBox dir_ray(geo::ray(curr_pos,new_pos),pos.transform.scale);

						voxtra::RayWorldCollision coll = collision::ray_box_cast(dir_ray, collision::HitQuery(object));
						
						if (!coll) {
							pos.transform.position = curr_pos + body.velocity * hit_toi*.99f;
							hit_toi = 0;
						}
						else {
							voxtra::RayWorldHit hit = coll.unwrap();
							stn::Option<math::Direction3d> collision_dir_mabye = hit.hit_direction();
							double move_time = hit_toi*hit.hit.length() / dir_ray.ray.length();
							
							if (!collision_dir_mabye) {
								v3::Vec3 offset = geo::collide_box(dir_ray.start_box().expanded(.001f), hit.collider.get<aabb::Collider>().global_box()).unwrap();
								pos.transform.position += offset;
								break;
							}
							v3::Vec3 collision_normal = collision_dir_mabye.unwrap().vec();
							hit_toi -= move_time;
							
							v3::Point3 hit_pos= coll.unwrap().intersection() + collision_normal*1e-3;
							pos.transform.position = hit_pos;
							v3::Vec3 vel_normal = collision_normal*v3::dot(body.velocity, collision_normal);
							body.velocity-=vel_normal;
						}
				}
				body.velocity += body.acceleration * deltaTime;
				body.acceleration = Vec3(0, 0, 0);
			}

		}

	};

	struct PhysicsPlugin :Core::Plugin {
		void build(Core::App& world) {

			world.insert_plugin<timename::TimePlugin>();
			world.emplace_system< RigidbodySystem>();
		}
	};
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