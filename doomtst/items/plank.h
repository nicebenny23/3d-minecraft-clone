#include "item.h"
#include "../block/block.h"
#ifndef plankitem_HPP
#define plankitem_HPP
#define plankitemblockid -1
inline void initplankitem(item* itm) {
	itm->properties.pickaxepower = 1;

	itm->maxamt = 64;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\treestoneblock.png");
}
#endif // !grassitem_HPP
