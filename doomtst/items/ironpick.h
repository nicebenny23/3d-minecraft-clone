#include "item.h"
#ifndef ironpickitem_HPP
#define ironpickitem_HPP
#define ironpickblockid -1
inline void ironpickinit(item* itm) {
	itm->pickaxepower = 5;
	itm->placeable = false;
	itm->maxamt = 64;
	itm->amt = 64;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\ironpick.png");
}

#endif // !ironpickitem_HPP
