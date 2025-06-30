
#include "../block/block.h"
#include "item.h"
#pragma once 
#define ropeitemasblock blockname::minecraftrope
inline void initropeitem(item* itm) {

	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;
	itm->properties.dmg = 0;
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\rope.png","RopeItemTexture");

}



 // !ropeitem_HPP
