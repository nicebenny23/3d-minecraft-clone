#include "item.h"
#pragma once 
#define lavapickblockid -1
inline void lavapickinit(item* itm) {
	itm->properties.pickaxepower = 3;
	itm->properties.placeable = false;
	itm->maxamt = 64;
	itm->amt = 64;
	itm->itemtype = wear;
	itm->itemui.itemsprite= createitembox("images\\pikaxe.png","PickaxeTexture");
}

 // !lavapickitem_HPP
