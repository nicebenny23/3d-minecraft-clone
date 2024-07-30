#include "item.h"
#ifndef ironitem_HPP
#define ironitem_HPP
#define ironitemasblockid -1

inline void initironitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->placeable = true;
	itm->itemui.itemsprite = createitembox("images\\iron.png");
}


#endif // !stoneitem_HPP
