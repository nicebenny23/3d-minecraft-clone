#include "item.h"

#ifndef ironsworditem_HPP
#define ironsworditem_HPP
#define ironsworditemblockid -1
inline void initironsworditem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->properties.placeable = false;
	itm->maxamt = 100;
	itm->properties.dmg = 6;

	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\ironsword.png","IronSwordTexture");
}
#endif // !ironsworditem_HPP
