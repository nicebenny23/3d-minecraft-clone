#include "item.h"
#include "../block/block.h"
#ifndef sanditem_HPP
#define sanditem_HPP
#define sanditemblockid blockname::minecraftsand
inline void sanditeminit(item* itm) {
	itm->properties.pickaxepower = 2;
	itm->properties.placeable = true;
	itm->maxamt = 41;
	itm->amt = 4;
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\sand.png","SandTexture");
}

#endif // !rockitemitem_HPP
