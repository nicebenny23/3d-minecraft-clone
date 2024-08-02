#include "item.h"
#include "../block/block.h"
#ifndef treestoneitem_HPP
#define treestoneitem_HPP
#define treestoneitemblockid blockname::minecrafttreestone
inline void inittreestoneitem(item* itm) {
	itm->properties.pickaxepower = 1;
	
	itm->maxamt = 64;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\treestone.png");
}
#endif // !grassitem_HPP
