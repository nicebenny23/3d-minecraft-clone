#include "item.h"
#include "../block/block.h"
#ifndef dirtitem_HPP
#define dirtitem_HPP
#define dirtitemblockid blockname::minecraftdirt
inline void initdirtitem(item* itm) {
	itm->pickaxepower = 1;
	
	itm->maxamt = 64;
	itm->placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\dirt.png");
}
#endif // !grassitem_HPP
