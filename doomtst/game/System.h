#include "gameobject.h"
#include "query.h"
#pragma once

struct System {

	virtual void run(gameobject::Ecs* ecs) {


	}

};


struct SystemExecutor {
	SystemExecutor(gameobject::Ecs* Ecs):ecs(Ecs) {


	}
	SystemExecutor() {


	}
	template<typename T>
	void push(T* sys) {
		static_assert(std::is_base_of<System, T>::value, "T must be a system");
		if (!type_list.insert<T>().second)
		{
			throw std::logic_error("Systems Must be singleton");
		}
		dependency_executor.push<T>();
		sys_list.push(sys);
	}
	void Run() {
		
		for (size_t ind:dependency_executor.sortedActive)
		{
			sys_list[ind]->run(ecs);
		}

	}
	gameobject::Ecs* ecs;
	Depends::DependencySystem dependency_executor;
	Cont::array<System*> sys_list;
	type_id::type_indexer type_list;

};
