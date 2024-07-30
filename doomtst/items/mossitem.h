#include "item.h"
#include "../block/block.h"
#ifndef mossitem_HPP
#define mossitem_HPP
#define mossitemblockid blockname::minecraftlava
inline void initmossitem(item* itm) {
	itm->pickaxepower = 1;
	itm->foodval = 2;
	itm->maxamt = 1;
	itm->placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\mossitem.png");
}
#endif // !grassitem_HPP
