#include "../../util/queue.h"
#include "../../util/type_index.h"
#include "../../util/dynpool.h"
#include "../../util/dependency.h"
#pragma once
namespace ecs {
	struct Ecs;
	struct Command {
		virtual ~Command() = default;
		virtual void apply(Ecs& world) = 0;

	};
	struct WorldCommand: Command{
		virtual void apply(Ecs& world) = 0;

	};
	template<typename T>
		concept CommandType =
		std::derived_from<T,Command>&&
		requires(T a, Ecs& world) {
			{ a.apply(world) } -> std::same_as<void>;
	};
	template<typename T>
	concept WorldCommandType =
		std::derived_from<T, WorldCommand>&&
		requires(T a, Ecs& world) {
			{ a.apply(world) } -> std::same_as<void>;
	};
	
	struct CommandPool {
		stn::flux store;
		stn::array<stn::flux_token<Command>> queue;
		void run_on(Ecs& World) {
			while (!queue.empty())
			{
				stn::flux_token<Command> cmd = queue.pop();
				cmd.get_ptr()->apply(World);
			}

		}
		template<CommandType T, typename...Args>
		void emplace(Args&& ...args) {
			stn::flux_token<T> command = store.emplace_unchecked<T>(std::forward<Args>(args)...);
			queue.push(command.abstract<Command>());

		}
		template<CommandType T>
		CommandPool(std::type_identity<T>) : store(std::type_identity<T>()) {}
		size_t commands() const {
			return queue.length();
		}
		size_t empty() const {
			return queue.empty();
		}
	};
	struct Commands {
		stn::array<CommandPool> pools;
		inline void run_on(Ecs& world) {
		
			stn::array<size_t> indices = DependencySystem.sortedActive;
			for (auto& index : indices)
			{
				pools[index].run_on(world);
			}

		}
		template<CommandType T>
		size_t insert_id() {
			auto [Id, is_new] = command_types.insert<T>();
			if (is_new) {
				DependencySystem.push<T>();
				pools.emplace(std::type_identity<T>());
			}
			return Id.id;
		}
		template<CommandType T, typename ...Args>
		void emplace(Args&&... args)
		{
			pools[insert_id<T>()].emplace<T>(std::forward<Args>(args)...);
		}
		template<CommandType T>
		void push(T&& value)
		{
			emplace<T>(std::forward<T>(value));
		}
		
		Commands():DependencySystem(), command_types() {
			
		}
		type_map::type_indexer<> command_types;
		Depends::DependencySystem DependencySystem;

	};


}