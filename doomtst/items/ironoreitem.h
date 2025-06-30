#include "item.h"
#include "../block/block.h"
#pragma once 
#define ironoreitemasblockid blockname::minecraftironore

inline void initironoreitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\ironore.png","IronOreTexture");
}


 // !stoneitem_HPP
