#include "item.h"
#include "../block/block.h"
#pragma once 
#define obsidianasblockid blockname::minecraftobsidian

inline void initobsidianitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\obb.png","ObsidianTexture");
}


 // !stoneitem_HPP
