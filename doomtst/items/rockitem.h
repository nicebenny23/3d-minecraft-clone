#include "item.h"
#ifndef rockitem_HPP
#define rockitemitem_HPP
#define rockitemblockid -1
inline void rockiteminit(item* itm) {
	itm->pickaxepower = 2;
	itm->placeable = false;
	itm->maxamt = 4;
	itm->amt = 4;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\rock.png");
}

#endif // !rockitemitem_HPP