#pragma once
#include "ecs.h"
namespace ecs{
	template<CommandType... Cs>
	struct composite_command {
		std::tuple<Cs...> commands;
		using Dependencies = TypeList::concat_all_t<typename Depends::GetDependencies<Cs>...>;

	private:
		// Compute if any subcommand has a first/last tag
		static constexpr bool has_first = (Depends::has_first_tag<Cs>|| ...);
		static constexpr bool has_last = (Depends::has_last_tag<Cs> || ...);
		
		// Compile-time check: cannot mix first and last
		static_assert(!(has_first&& has_last),
			"Composite command cannot contain both dependency_first and dependency_last commands");

	public:
		// Effective tag for the composite
		using DependencyTag = std::conditional_t<
			has_first, Depends::dependency_first,
			std::conditional_t<has_last, Depends::dependency_last, void>
		>;

		// Perfect forwarding with exact type matching
		template<typename... Ts>
			requires (sizeof...(Ts) == sizeof...(Cs) &&
		((std::same_as<std::remove_cvref_t<Ts>, Cs>) && ...))
			composite_command(Ts&&... ts)
			: commands(std::forward<Ts>(ts)...) {
		}

		void apply(Ecs& world) {
			std::apply([&](auto&... c) { (c.apply(world), ...); }, commands);
		}
	};

	template<CommandType Cmd>
	struct command_view {
		Cmd cmd;
		template<CommandType Next>
		command_view<composite_command<Cmd, Next>> then(Next&& next)&& {
			static_assert(!Depends::is_before<Next,Cmd>(), "Next may not be ordered before Cmd");
			return command_view(composite_command<Cmd, Next>(std::move(cmd), std::forward<Next>(next)));
		}
		//since these 
		template<CommandType Next>
		command_view<composite_command<Cmd, Next>> then(Next&& next) const& {
			static_assert(!Depends::is_before<Next, Cmd>(), "Next may not be ordered before Cmd");
			return command_view(composite_command<Cmd, Next>(cmd, std::forward<Next>(next)));
		}

		command_view(Cmd&& command) :cmd(std::move(command)) {

		}
		command_view(const Cmd& command) :cmd(command) {}

		void apply(Ecs& world) {
			cmd.apply(world);
		}
	};
	struct DestroyEntity :Command {
		space_id ent;
		void apply(Ecs& world) {
			if (world.contains(ent))
			{
				world.remove_object_unchecked(ent);
			}
		}
		DestroyEntity(space_id object):ent(object) {
		}
	};
	struct DestroyComponent :Command {
		component_id id;
		space_id owning_entity;
		void apply(Ecs& world) {
			if (!world.contains(owning_entity))
			{
				return;
			}
			if (world.has_component(owning_entity, id)) {
				return;
			}
			world.remove_component_unchecked(owning_entity, id);
		}
		
		DestroyComponent(space_id ent,component_id comp_id) :owning_entity(ent), id(comp_id) {

		}
	};

}