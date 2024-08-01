#include "item.h"
#ifndef tpsworditem_HPP
#define tpsworditem_HPP
#define tpswordblockid -1
inline void initfinalsworditem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 1024;
	
	itm->properties.dmg = 10;

	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\finalsword.png");
}
#endif // !ironsworditem_HPP
