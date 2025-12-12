#include "../util/queue.h"
#include "../util/type_index.h"
#include "../util/dynpool.h"
#include "../util/dependency.h"
#pragma once
namespace ecs {
	struct Ecs;
}

struct Command {
	Command() {}; 
	virtual void Apply(ecs::Ecs* world) = 0; 
	
}
; template<typename T>
concept CommandType =
std::is_base_of_v<Command, T>&&
	requires(T a, ecs::Ecs* world) {
		{ a.Apply(world) } -> std::same_as<void>;
};

struct CommandPool {
	stn::flux store;
	stn::array<stn::flux_token<Command>> queue;
	void apply(ecs::Ecs* World) {
		while (!queue.empty())
		{
			stn::flux_token<Command> cmd = queue.pop();
			cmd.get_ptr()->Apply(World);
		}
		
	}
	template<CommandType T,typename...Args>
	void emplace(Args&& ...args) {
		stn::flux_token<T> command = store.emplace_unchecked<T>(std::forward<Args>(args)...);
		queue.push(command.abstract<Command>());

	}
	template<CommandType T>
	CommandPool(std::type_identity<T> ): store(std::type_identity<T>()){}
	size_t commands() const{
		return queue.length();
	}
	size_t empty() const {
		return queue.empty();
	}
};
struct CommandBuffer {
	stn::array< CommandPool> pools;
	inline void pop() {
		
	stn::array<std::uint32_t> indices=stn::array<std::uint32_t>(DependencySystem.sortedActive);
		for (auto& index:indices)
		{
			pools[index].apply(World);
		}

	}
	template<CommandType T>
	size_t insert_id() {
		auto [Id, is_new] = commandSystem.insert<T>();
		if (is_new) {
			DependencySystem.push<T>();
			pools.emplace(std::type_identity<T>());
			}
		return Id.id;
	}
	template<CommandType T>
	void push(const T& value)
	{

		pools[insert_id<T>()].emplace<T>(value);
	}

	CommandBuffer(ecs::Ecs* world):World(world){
	}
	CommandBuffer() {
		World = nullptr;
	}
	
	ecs::Ecs* World;
	stn::type_indexer<> commandSystem;
	Depends::DependencySystem DependencySystem;
	
}; 
