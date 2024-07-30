#include "item.h"
#include "../block/block.h"
#ifndef craftingtableitem_HPP
#define craftingtableitem_HPP
#define craftingtableblockid 9
inline void initcraftingtable(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemtype = count;

	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\craftingtabletop.png");
}
#endif // !grassitem_HPP
