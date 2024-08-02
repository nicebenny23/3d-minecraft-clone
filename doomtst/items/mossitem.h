#include "item.h"
#include "../block/block.h"
#ifndef mossitem_HPP
#define mossitem_HPP
#define mossitemblockid -1
inline void initmossitem(item* itm) {
	itm->properties.pickaxepower = 1;
	itm->properties.foodval = 2;
	itm->maxamt = 1;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\mossitem.png");
}
#endif // !grassitem_HPP
