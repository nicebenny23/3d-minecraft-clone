
#include "../game/ecs/query.h"
#include "../game/ecs/traversal.h"
#include "../math/transform.h"
#include "../game/Core.h"
#pragma once
namespace core {

	struct LocalTransform:ecs::component {
		LocalTransform(v3::Point3 point):transform(point,math::Look3(), v3::Scale3()) {
		
		}
		math::Transform transform;
	};
	struct GlobalTransform :ecs::component {
		glm::mat4 global_matrix;
	};
	struct GlobalTransformer : ecs::System{
		void run(ecs::Ecs& world) {
			ecs::View<LocalTransform, GlobalTransform,ecs::Not<ecs::Child>,ecs::Owner> top_level_transforms(world);
			stn::queue < ecs::Constrained<LocalTransform, GlobalTransform>> transforms;
			for (auto&& [child,transform,object]:top_level_transforms) {
				transforms.push(object);
			}
			while (transforms.empty()) {
				ecs::Constrained<LocalTransform, GlobalTransform> transform = transforms.pop();
				stn::Option<ecs::obj> parent= ecs::parent(transform.object());
				if (parent) {
					transform.get<GlobalTransform>().global_matrix =
					parent.
					map([](ecs::obj par) {return par.get_component<GlobalTransform>().global_matrix; })
					.unwrap_or(glm::mat4(1))* transform.get<LocalTransform>().transform.as_matrix();
				}
				for (ecs::entity entity:ecs::children(transform.object())) {
					transforms.push(ecs::obj(entity, world));
				}
			}
		}
	};

	struct TransformPlugin {
		void operator()(Core::App& world) {
			world.emplace_system< GlobalTransformer>();
		}
	};
	struct TransformRecipe {
		TransformRecipe(v3::Point3 pnt) :center(pnt) {

		}
		v3::Point3 center;
		void apply(ecs::obj& object) const {
			object.add_component<LocalTransform>(center);
			object.add_component<GlobalTransform>();
		}
	};
}