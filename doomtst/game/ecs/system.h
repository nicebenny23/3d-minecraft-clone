#pragma once
#include "component.h"

#include "../../util/unique.h"
#include "../../util/pair.h"
#include <concepts>
#include "resources.h"
#include "../../util/dependency.h"
namespace ecs {
	struct System {
		virtual ~System() = default;
		virtual void run(Ecs& ecs) = 0;
	};

	template<typename T>
	concept SystemType = std::derived_from<T,System>;

	struct Systems:ecs::resource {
		Systems():dependency_executor(){
		}		
		
		template<SystemType T,typename ...Args> requires std::constructible_from<T,Args&&...>
		void emplace(Args&&... args ) {
			stn::insertion<stn::Id> insertion = types.insert<T>();
			if (insertion.is_new) {
				stored_systems.push(stn::box<T>(std::forward<Args>(args)...).upcast<System>());
				dependency_executor.push<T>();
			}
		}
		
		size_t count() const {
			return stored_systems.length();
		}
		void run_on(Ecs& ecs) {
			for (size_t ind : dependency_executor.sortedActive)
			{
				stored_systems[ind]->run(ecs);
			}
		}
		Depends::DependencySystem dependency_executor;
		stn::array<stn::box<System>> stored_systems;
		stn::type_indexer<> types;
		
	};
}