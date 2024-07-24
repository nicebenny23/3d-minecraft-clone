#include "item.h"
#ifndef lavapickitem_HPP
#define lavapickitem_HPP
#define lavapickblockid -1
inline void lavapickinit(item* itm) {
	itm->pickaxepower = 5;
	itm->placeable = false;
	itm->maxamt = 64;
	itm->amt = 64;
	itm->itemtype = wear;
	itm->itemui.itemsprite= createitembox("images\\pikaxe.png");
}

#endif // !lavapickitem_HPP
