#include "item.h"
#pragma once 
#define irontopitemblockid -1
inline void initirontopitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 100;
	itm->properties.dmg = 0;

	itm->properties.armor = .8;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\irontop.png","IronTopTexture");
}
 // !ironsworditem_HPP
