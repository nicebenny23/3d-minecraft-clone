#pragma once
#include "component.h"

#include "../../util/unique.h"
#include "../../util/pair.h"
#include <concepts>
#include "../../util/dependency.h"
namespace ecs {
	struct System {
		virtual ~System() = default;
		virtual void run(Ecs& ecs) = 0;
	};

	template<typename T>
	concept SystemType = std::derived_from<T,System>;

	struct Systems {
		Systems():dependency_executor(){
		}		
		
		template<SystemType T,typename ...Args> requires std::constructible_from<T,Args&&...>
		void emplace(Args&&... args ) {
			stored_systems.push(stn::box<T>(std::forward<Args>(args)...).upcast<System>());
			insert<T>(*(T*)stored_systems.last().get());
		}
		
		template<SystemType T>
		void insert(T& sys) {
			stn::insertion<stn::Id> insertion= types.insert<T>();
			if (insertion.is_new)
			{
				dependency_executor.push<T>();
				sys_list.push(&sys);
			}
			else
			{
				sys_list[insertion.value.id] = &sys;
			}
		}
		size_t count() const {
			return sys_list.length();
		}
		void run_on(Ecs& ecs) {

			for (size_t ind : dependency_executor.sortedActive)
			{
				sys_list[ind]->run(ecs);
			}

		}
		Depends::DependencySystem dependency_executor;
		stn::array<System*> sys_list;
		stn::array<stn::box<System>> stored_systems;
		stn::type_indexer<> types;

	};
}