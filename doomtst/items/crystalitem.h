#include "item.h"
#pragma once 
#define crystalitemasblockid -1

inline void initcrystalitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\crystal.png","CrystalTexture");
}


 // !stoneitem_HPP
