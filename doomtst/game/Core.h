#pragma once
#include "../game/ecs/ecs.h"
#include "../util/dynamicarray.h"
#include <utility>
//start of the application
namespace Core {
	struct App;
	template<typename T>
	concept PluginType = std::constructible_from<stn::Stateless<void(App&)>,T&&>;
	
	using plugin_ty=stn::Stateless<void(App&)>;
	struct Plugins {
		stn::array<plugin_ty> plugin_list;

		template<PluginType T>
		void insert(T&& plugin) {
			if (!plugin_list.contains(plugin_ty(plugin))) {
				plugin_list.push(plugin);
				plugin(*engine);
			}
		}
		Plugins(App& app) :engine(app) {}
	private:
		stn::non_null<App> engine;
	};
	
	struct CloseGameCommand {

	};
	

    struct App
    {

        Plugins plugin_list;
		template<PluginType T>
		void insert_plugin(T&& plugin) {
			plugin_list.insert(plugin);
		}

		template<ecs::ResourceType T,typename ...Args>
		T& emplace_resource(Args&&... args) requires std::constructible_from<T,Args...>{
			return Ecs.insert_resource<T>(std::forward<Args>(args)...);
		}
		template<ecs::ResourceType T>
		T& ensure_resource() {
		return Ecs.insert_resource<T>();
		}
		template<ecs::SystemType T, typename ...Args>
		void emplace_system(Args&&... args) requires std::constructible_from<T, Args&&...>||(std::constructible_from<T, ecs::Ecs&>) {
			Ecs.emplace_system<T>(std::forward<Args>(args)...);
		}

		App():Ecs(),plugin_list(*this) {

		}
        ecs::Ecs Ecs;
		
    };
	template<ecs::SystemType T>
	struct AddSystemPlugin{
		void build(App& engine) {
			engine.emplace_system<T>();
		};

	};
    extern App game;
	struct GameState :ecs::resource {
		GameState() :should_close(false) {

		}
		bool should_close;
	};
	struct GameCloser :ecs::System {
		void run(ecs::Ecs& world) {
			for (CloseGameCommand cmd : world.read_commands<CloseGameCommand>()) {
				world.get_resource<GameState>().should_close = true;
			}
		}

	};
	struct GamePlugin {
		void operator()(App& app) {
			app.ensure_resource <GameState>();
			app.emplace_system<GameCloser>();
		}
	};
}


