#include "gameobject.h"
#include "../util/dynamicarray.h"
#pragma once
namespace gameobject {
	namespace spawn {
		struct Spawner {

			explicit Spawner(Ecs* ecs) : ecs(ecs) {};
	
			template<typename Comp, typename... Args>
			Spawner& with(Args&&... args) {
				pending.push([argsTuple = std::make_tuple(std::forward<Args>(args)...)](obj e) mutable {
					std::apply([&e](auto&&... params) {
						e.addcomponent<Comp>(std::forward<decltype(params)>(params)...);
						}, argsTuple);
					});
				return *this;
			}
			obj build() {
				obj new_obj=ecs->create_entity();
				for (auto& entity_function:pending)
				{
					entity_function(new_obj);
				}
				pending.destroy();
				return new_obj;
			}
		private:
			Ecs* ecs;
			stn::array<std::function<void(obj)>> pending;
		};
	

	}

}