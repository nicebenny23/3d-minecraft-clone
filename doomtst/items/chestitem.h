#include "item.h"
#include "../block/block.h"
#ifndef chestitem_HPP
#define chestitem_HPP
#define chestblockid blockname::minecraftchest
inline void initchestitem(item* itm) {
	itm->properties.pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemtype = count;

	itm->properties.placeable = true;
	itm->itemui.itemsprite = createitembox("images\\chest.png");
}
#endif // !grassitem_HPP
