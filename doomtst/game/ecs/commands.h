#pragma once
#include "../../util/queue.h"
#include "../../util/type_index.h"
#include "../../util/dynpool.h"
#include "../../util/dependency.h"
#include "../../util/pipeline.h"
#include "resources.h"
namespace ecs {

	template<typename T>
	struct Commands:resource {
		decltype(auto) read() {
			return stn::drain(std::move(commands));
		}
		
		template<typename ...Args>
		void emplace(Args&&... args) {
			commands.emplace(std::forward<Args>(args)...);
		}
	private:
		stn::queue<T> commands;

	};


}