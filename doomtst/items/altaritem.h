#include "item.h"
#include "../block/block.h"
#ifndef altaritem_HPP
#define altaritem_HPP
#define altaritemblockid blockname::altaritem
inline void initaltaritem(item* itm) {
	itm->properties.pickaxepower = 1;

	itm->maxamt = 64;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\ultraaltar.png");
}
#endif // !grassitem_HPP
