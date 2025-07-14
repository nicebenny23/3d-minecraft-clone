#include "../util/queue.h"
#include "../util/type_index.h"
#include "../util/dynpool.h"
#include "../util/dependency.h"
#pragma once
namespace gameobject {
	struct Ecs;
}

struct Command {
	Command() {}; 
	virtual void Apply(gameobject::Ecs* world) = 0; 
	
}; 
using ApplyFn = void (*)(Command*, gameobject::Ecs*);
struct CommandPool {
	dynPool::flux<Command> store;
	Cont::queue<Command*> queue;
	ApplyFn CommandFunc;
	void apply(gameobject::Ecs* World) {
		
		while (!queue.empty())
		{
			Command* cmd = queue.pop();
			CommandFunc(cmd,World);

			store.free(cmd);
		}


	}

};
struct CommandBuffer {
	Cont::array<dynPool::flux<Command>> store;
	Cont::array<Cont::queue<Command*>> buffer;

	void pop();
	template<typename T>
	void push(const T& value);
	CommandBuffer(gameobject::Ecs* wrld):World(wrld){

	}
	CommandBuffer() {
		World = nullptr;
	}
	gameobject::Ecs* World;
	type_id::type_indexer commandSystem;
	Depends::DependencySystem DependencySystem;
	bool empty() {
		for (auto& count : buffer)
		{
			if (count.length () != 0) {
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
		store.push(dynPool::flux<Command>(sizeof(T),alignof(T)));
		DependencySystem.push<T>();
	}

	void* mem = store[Id.value].alloc();
	T* cmd = new (mem) T(value);
	buffer[Id.value].push(cmd);
}
inline void CommandBuffer::pop() {
	
	size_t length = DependencySystem.sortedActive.length;
	for (size_t ind = 0; ind <length; ind++) {
		empty_num(DependencySystem.sortedActive[ind]);
	}
	
}