#include "../util/fileloader.h"
#include <filesystem>
#pragma once
namespace World {
	namespace fs = std::filesystem;
	struct world{
		world() {
		}
		world(size_t key) {
			std::filesystem::path base = getWindowsHomeDir() / "benny_render_3d" / "worlds";
			world_path = createUniqueNamedFolder(base, "World");
			createFolder(world_path,"Chunks");
			int l = 1;
		}
		std::filesystem::path get_path() {
			return std::filesystem::path(world_path);
		}
	private:
		std::filesystem::path world_path;

	};
}