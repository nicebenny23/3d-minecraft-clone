#pragma once
#include "../renderer/Window.h"
#include "GameContext.h"
#include "../world/world.h"
//start of the application
namespace Core {
	struct App;
	struct Plugin {
		virtual void build(App& engine) = 0;
		Plugin() {
		}
	};
	using plugin_id = stn::typed_id<Plugin>;
	template<typename T>
	concept PluginType = std::derived_from<T, Plugin>&&std::is_default_constructible_v<T>;
	struct Plugins {
		stn::type_indexer<plugin_id> plugin_list;
		template<PluginType T>
		void insert() {
			if (plugin_list.insert<T>()) {
				T().build(*engine);
			}
		}
		void inject_engine(App& eng) {
			engine = &eng;
		}
		Plugins() :engine() {}
	private:
		App* engine;
	};
	struct CloseGameCommand {

	};
	

    struct App
    {

        void ConnectToContext();
        void createWindow();
        void CreateGrid();
        void InitInput();
        void InitOC();
        void CreateWorld();
        void InitRenderer();
       CtxName::Context* ctx;
   
        World::world world;
		Plugins plugin_list;
		template<PluginType T>
		void insert_plugin() {
			plugin_list.insert<T>();
		}

		template<ecs::ResourceType T,typename ...Args>
		T& emplace_resource(Args&&... args) requires std::constructible_from<T,Args...>{
			return Ecs.insert_resource<T>(std::forward<Args>(args)...);
		}
		template<ecs::ResourceType T>
		T& ensure_resource() {
		return Ecs.ensure_resource<T>();
		}
		template<ecs::SystemType T, typename ...Args>
		void emplace_system(Args&&... args) requires std::constructible_from<T, Args&&...>||(std::constructible_from<T, ecs::Ecs&>) {
			Ecs.emplace_system<T>(std::forward<Args>(args)...);
		}

		App():Ecs(size_t(1)<<21) {
			plugin_list.inject_engine(*this);
		}
        ecs::Ecs Ecs;
		
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
				world.get_resource<GameState>().unwrap().should_close = true;
			}
		}

	};
	struct GamePlugin :Core::Plugin {
		void build(App& app) {
			app.ensure_resource < GameState>();
			app.emplace_system< GameCloser>();
		}
	};
}


