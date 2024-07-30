#include "item.h"
#ifndef ironsworditem_HPP
#define ironsworditem_HPP
#define ironsworditemblockid 9
inline void initironsworditem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 100;
	itm->properties.dmg = 4;

	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\ironsword.png");
}
#endif // !ironsworditem_HPP
