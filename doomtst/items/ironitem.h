#include "item.h"
#pragma once 
#define ironitemasblockid -1

inline void initironitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\iron.png","IronTexture");
}


 // !stoneitem_HPP
