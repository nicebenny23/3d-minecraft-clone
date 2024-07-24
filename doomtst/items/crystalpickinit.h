#include "item.h"
#ifndef crystalpickitem_HPP
#define crystalpickitem_HPP
#define crystalpickblockid -1
inline void crystalpickinit(item* itm) {
	itm->pickaxepower = 10;
	itm->placeable = false;
	itm->maxamt = 200;
	itm->amt = 200;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\crystalpick.png");
}

#endif // !crystalpickitem_HPP
