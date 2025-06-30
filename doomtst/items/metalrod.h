#include "item.h"
#pragma once 
#define metalroditemasblockid -1

inline void initmetalroditem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\metalrod.png","MetalRodTexture");
}


 // !stoneitem_HPP
