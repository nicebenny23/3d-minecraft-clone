#include "item.h"
#include "../block/block.h"
#pragma once 
#define ironpickblockid blockname::minecraftwater
inline void ironpickinit(item* itm) {
	itm->properties.pickaxepower = 4.9;
	itm->properties.placeable = false;
	itm->maxamt = 256;
	itm->amt = 64;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\ironpick.png","IronPickTexture");
}

 // !ironpickitem_HPP
