#include "item.h"
#include "../block/block.h"
#ifndef ironoreitem_HPP
#define ironoreitem_HPP
#define ironoreitemasblockid blockname::minecraftironore

inline void initironoreitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->placeable = true;
	itm->itemui.itemsprite = createitembox("images\\ironore.png");
}


#endif // !stoneitem_HPP
