#include "item.h"
#ifndef stoneitem_HPP
#define stoneitem_HPP
#define stoneitemasblockid 3 

inline void initstoneitem(item* itm) {
	itm->pickaxepower = 1;
	itm->maxamt = 64;
	itm->itemtype = count;
	itm->placeable = true;
	itm->itemui.itemsprite = createitembox("images\\stone.png");
}


#endif // !stoneitem_HPP
