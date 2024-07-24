#include "item.h"
#ifndef crystalitem_HPP
#define crystalitem_HPP
#define crystalitemasblockid -1

inline void initcrystalitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->placeable = true;
	itm->itemui.itemsprite = createitembox("images\\crystal.png");
}


#endif // !stoneitem_HPP
