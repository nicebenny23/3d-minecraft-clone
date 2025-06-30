#include "item.h"
#pragma once 
#define crystalpickblockid -1
inline void crystalpickinit(item* itm) {
	itm->properties.pickaxepower = 7;
	itm->properties.placeable = false;
	itm->maxamt = 512;
	itm->amt = 256;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\crystalpick.png","CrystalPickaxeTexture");
}

 // !crystalpickitem_HPP
