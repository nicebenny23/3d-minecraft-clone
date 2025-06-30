#include "item.h"
#pragma once 
#define tpswordblockid -1
inline void initfinalsworditem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 512;
	
	itm->properties.dmg = 10;

	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\finalsword.png","FinalSwordTexture");
}
 // !ironsworditem_HPP
