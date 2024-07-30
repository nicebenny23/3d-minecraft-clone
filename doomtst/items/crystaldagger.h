#include "item.h"
#include "../block/block.h"
#ifndef crystaldaggeritem_HPP
#define crystaldaggeritem_HPP
#define crystaldaggeritemasblockid -1

inline void initcrystaldaggeritem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->placeable = true;
	itm->itemui.itemsprite = createitembox("images\\crystaldagger.png");
}


#endif // !stoneitem_HPP
