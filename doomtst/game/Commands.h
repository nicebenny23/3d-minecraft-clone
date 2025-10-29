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
	stn::queue<Command*> queue;
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
	stn::array<dynPool::flux<Command>> store;
	stn::array<stn::queue<Command*>> buffer;

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
	bool is_empty() const{
		for (auto& count : buffer)
		{
			if (!count.empty()) {
				return false;
			}
		}
		return true;
	}
	void consume_at(size_t index) {
		auto& queue = buffer[index];
		auto& storage = store[index];

		while (!queue.empty())
		{
			Command* cmd = queue.pop();
			cmd->Apply(World);
			
			storage.free(cmd);
		}
		

	}
	size_t total_commands() const{
		size_t count=0;
		for (int i = 0; i < buffer.length(); i++)
		{
			count += buffer[i].length();
		}
		return count;
	}
	size_t total_buffers() const {
		return buffer.length();

	}
	
}; 
template<typename T>
void CommandBuffer::push(const T& value)
{
	static_assert(std::is_base_of<Command, T>::value, "T must derive from Command");
	auto [Id, is_new] = commandSystem.insert<T>();
	if (is_new) {
		buffer.push(stn::queue<Command*>());
		store.push(dynPool::flux<Command>(stn::memory::layout_of<T>));
		DependencySystem.push<T>();
	}

	void* mem = store[Id.id].alloc();
	T* cmd = new (mem) T(value);
	buffer[Id.id].push(cmd);
}
inline void CommandBuffer::pop() {
	
	for (size_t ind = 0; ind < total_buffers(); ind++) {
		consume_at(DependencySystem.sortedActive[ind]);
	}
	
}