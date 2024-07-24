#include "item.h"
#ifndef sworditem_HPP
#define sworditem_HPP
#define sworditemblockid 9
inline void initsworditem(item* itm) {
	itm->pickaxepower = 0;
	itm->placeable = false;
	itm->maxamt = 3;
	itm->dmg = 4;

	itm->itemtype = wear;
itm->itemui.itemsprite = createitembox("images\\sword.png");
}
#endif // !sworditem_HPP
