
#include "item.h"
#pragma once 
#define crystaltopitemblockid -1
inline void initcrystaltopitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 256;

	itm->properties.dmg = 0;

	itm->properties.armor = .7;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\crystaltop.png","CrystalTopTexture");
}
 // !ironsworditem_HPP
