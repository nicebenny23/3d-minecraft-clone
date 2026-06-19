#include "../util/fileloader.h"
#include <filesystem>
#include "../game/ecs/ecs.h"
#include "../game/ecs/resources.h"
#pragma once
namespace grid {
	namespace fs = std::filesystem;
	//the world resource holds the file location of the current world state
	struct World:ecs::resource{
		World() {
		}
		World(size_t key) {
			std::filesystem::path base = getWindowsHomeDir() / "benny_render_3d" / "worlds";
			world_path = createUniqueNamedFolder(base, "grid");
			createFolder(world_path,"Chunks");
			int l = 1;
		}
		std::filesystem::path get_path() {
			return std::filesystem::path(world_path);
		}
		std::filesystem::path augment() {
			return std::filesystem::path(world_path);
		}

	private:
		std::filesystem::path world_path;

	};
}