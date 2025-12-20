#pragma once
#include "../renderer/Window.h"
#include "../world/grid.h"
#include "../util/userinput.h"
#include "GameContext.h"
#include "entity.h"
#include "../renderer/renderer.h"
#include "../util/time.h"
#include "../renderer/guirender.h"
#include "../world/world.h"
//start of the application
namespace Core {
	struct Engine;
	struct Plugin {
		virtual void Build(Engine& engine) = 0;
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
				T().Build(*engine);
			}
		}
		void inject_engine(Engine& eng) {
			engine = &eng;
		}
		Plugins() :engine() {}
	private:
		Engine* engine;
	};
    struct Engine
    {

        void ConnectToContext();
        void createWindow();
        void CreateGrid();
        void InitTime();
        void InitInput();
        void InitOC();
        void CreateWorld();
        void InitRenderer();
       CtxName::Context* ctx;
        timename::TimeManager Time;
        userinput::InputManager Inp;
        window::Window Window;
        renderer::Renderer* ren;
        World::world world;
        grid::Grid* Grid;
		Plugins plugin_list;
		template<PluginType T>
		void insert_plugin() {
			plugin_list.insert<T>();
		}
		Engine():Ecs(2<<18) {
			plugin_list.inject_engine(*this);
		}
        ecs::Ecs Ecs;
    };
    extern Engine game;
    
}


