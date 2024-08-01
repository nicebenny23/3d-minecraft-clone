#include "item.h"
#include "../block/block.h"
#ifndef obsidianitem_HPP
#define obsidianitem_HPP
#define obsidianasblockid blockname::minecraftobsidian

inline void initobsidianitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\obb.png");
}


#endif // !stoneitem_HPP
