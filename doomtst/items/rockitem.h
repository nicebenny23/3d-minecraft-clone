#include "item.h"
#ifndef rockitem_HPP
#define rockitem_HPP
#define rockitemblockid 
inline void rockiteminit(item* itm) {
	itm->properties.pickaxepower = 2;
	itm->properties.placeable = false;
	itm->maxamt = 4;
	itm->amt = 4;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\rock.png");
}

#endif // !rockitemitem_HPP
