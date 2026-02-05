#include "managegrid.h"
#include "../block/blockinit.h"
void grid::GridManager::set_block(Coord loc, block_id blockid, grid::Grid& grid, ecs::Ecs& world) {
	block* location = grid.getBlock(loc);
	if (location != nullptr) {
		grid.GetChunk(loc)->modified = true;
		dislocate_from_grid(location->owner(), blockid);

		blockchangecoverupdate(world, location);
		if (location->owner().has_component<block_emmision>()) {
			world.write_command(grid::SpreadLightCommand());
		}
		world.write_command(grid::partial_reshade_command(grid, loc));
	}
}
