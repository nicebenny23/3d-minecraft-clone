#pragma once
#include "../world/world.h"
#include "../game/ecs/ecs.h"
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
	concept PluginType = std::derived_from<T, Plugin>&&std::is_default_constructible_v<T>||stn::void_invokable<const T&,App&>;
	template<PluginType T>
	void build_plugin(T& plugin,App& app) {
		if constexpr(stn::void_invokable<const T&, App&>) {
			plugin(app);
		}
		else {
			plugin.build(app);
		}
	}
	struct Plugins {
		stn::type_indexer<plugin_id> plugin_list;

		template<PluginType T>
		void insert(T& plugin) {
			if (plugin_list.insert<T>()) {
				build_plugin(plugin,*engine);
			}
		}
		template<PluginType T>
		void insert() {
			T plugin= T();
			insert<T>(plugin);
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

        void createWindow();
        void InitOC();
       Plugins plugin_list;
		template<PluginType T>
		void insert_plugin() {
			plugin_list.insert<T>();
		}

		template<PluginType T>
		void insert_plugin(T&& plugin) {
			plugin_list.insert<T>(std::forward<T>(plugin));
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

		App():Ecs() {
			plugin_list.inject_engine(*this);
		}
        ecs::Ecs Ecs;
		
    };
	template<ecs::SystemType T>
	struct AddSystemPlugin :Plugin {
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
	struct GamePlugin :Core::Plugin {
		void build(App& app) {
			app.ensure_resource < GameState>();
			app.emplace_system< GameCloser>();
		}
	};
}


