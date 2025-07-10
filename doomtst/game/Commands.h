#include "../util/queue.h"
#include "../util/type_index.h"
#include "../util/dynpool.h"
#include "../util/dependency.h"
#pragma once
namespace gameobject {
	struct ecs;
}

struct Command {
	Command() {}; 
	virtual void Apply(gameobject::ecs* World) = 0; 
	virtual ~Command() = default;
};
struct CommandPool {
	dynPool::flux<Command> store;
	Cont::queue<Command*> Commands;
	

};
struct CommandBuffer {
	Cont::array<dynPool::flux<Command>> store;
	Cont::array<Cont::queue<Command*>> buffer;

	void pop();
	template<typename T>
	void push(const T& value);

	gameobject::ecs* World;
	type_id::dense_type_system commandSystem;
	Depends::DependencySystem DependencySystem;
	bool empty() {
		for (auto& count : buffer)
		{
			if (count.size() != 0) {
				return false;
			}
		}
		return true;
	}
	void empty_num(size_t index) {
		auto& queue = buffer[index];
		auto& storage = store[index];

		while (!queue.empty())
		{
			Command* cmd = queue.pop();
			cmd->Apply(World);
			cmd->~Command();
			storage.free(cmd);
		}
		

	}
}; 
template<typename T>
void CommandBuffer::push(const T& value)
{
	static_assert(std::is_base_of<Command, T>::value, "T must derive from Command");
	auto [Id, is_new] = commandSystem.insert<T>();
	if (is_new) {
		buffer.push(Cont::queue<Command*>());
		store.push(dynPool::flux(sizeof(T)));
		DependencySystem.push<T>();
	}

	void* mem = store[Id.value].alloc();          
	Command* cmd = new (mem) T(value);             
	buffer[Id.value].push(cmd);                  
}
void CommandBuffer::pop() {
	            
	size_t length = DependencySystem.sortedActive.length;
	for (size_t ind = 0; ind <length; ind++) {
		empty_num(DependencySystem.sortedActive[ind]);
	}
}