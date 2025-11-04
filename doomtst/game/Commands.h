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
	
}
; template<typename T>
concept CommandType =
std::is_base_of_v<Command, T>&&
	requires(T a, gameobject::Ecs* world) {
		{ a.Apply(world) } -> std::same_as<void>;
};

using ApplyFn = void (*)(Command*, gameobject::Ecs*);
struct CommandPool {
	stn::flux store;
	stn::array<stn::flux_token<Command>> queue;
	ApplyFn CommandFunc;
	void apply(gameobject::Ecs* World) {
		
		while (!queue.empty())
		{
			stn::flux_token<Command> cmd = queue.pop();
			CommandFunc(&*cmd,World);

		}


	}
	template<CommandType T>
	void push(T&& cmd) {
		stn::flux_token<T> command = store.emplace<T>(cmd);
		queue.push(command.downcast<Command>());
	}
	template<typename T>
	CommandPool(): store(std::type_identity<T>()){}
	size_t commands() const{
		return queue.length();
	}
	size_t empty() const {
		return queue.empty();
	}

};
struct CommandBuffer {
	stn::array<stn::flux> store;
	stn::array<stn::array<stn::flux_token<Command>>> buffer;
	stn::array< CommandPool> pools;

	inline void pop() {

		for (size_t ind = 0; ind < total_buffers(); ind++) {
			consume_at(DependencySystem.sortedActive[ind]);
		}

	}
	template<CommandType T>
	void push(const T& value)
	{
		static_assert(std::is_base_of<Command, T>::value, "T must derive from Command");
		auto [Id, is_new] = commandSystem.insert<T>();
		if (is_new) {
			buffer.push(stn::array<stn::flux_token<Command>>());
			store.push(stn::flux(std::type_identity<T>()));
			DependencySystem.push<T>();
		}

		stn::flux_token<T> cmd = store[Id.id].emplace_unchecked<T>(value);
		buffer[Id.id].push(cmd.abstract<Command>());
	}

	CommandBuffer(gameobject::Ecs* wrld):World(wrld){

	}
	CommandBuffer() {
		World = nullptr;
	}
	gameobject::Ecs* World;
	type_map::type_indexer<> commandSystem;
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
			Command& cmd = *queue.pop();
			cmd.Apply(World);
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
