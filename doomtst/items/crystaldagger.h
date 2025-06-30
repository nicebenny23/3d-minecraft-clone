#include "item.h"
#include "../block/block.h"
#pragma once 
#define crystaldaggeritemasblockid -1

inline void initcrystaldaggeritem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\crystaldagger.png","CrystalDaggerTexture");
}


 // !stoneitem_HPP
