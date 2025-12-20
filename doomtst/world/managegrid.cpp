#include "managegrid.h"
#include "../block/blockinit.h"

void gridutil::GridManager::set_block(Coord loc, int blockid, grid::Grid& grid) {
	block* location = CtxName::ctx.Grid->getBlock(loc);
	if (location != nullptr) {
		int prevemit = CtxName::ctx.Grid->getBlock(loc)->emitedlight;
		CtxName::ctx.Grid->GetChunk(loc)->modified = true;
		set_air(location->owner());
		location = CtxName::ctx.Grid->getBlock(loc);
		location->id = blockid;
		blkinitname::blockinit(location);
		
		blockchangecoverupdate(*CtxName::ctx.Ecs,location);
		blocklightingupdateevent(prevemit, location->emitedlight, loc);
		sendrecreatemsg(grid);
	}

}
