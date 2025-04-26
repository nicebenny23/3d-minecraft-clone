#include "item.h"
#include "../block/block.h"
#ifndef glassbottleitem_HPP
#define glassbottleitem_HPP
#define glassbottleitemblockid -1
inline void glassbottleiteminit(item* itm) {
	itm->properties.pickaxepower = 2;
	itm->properties.placeable = true;
	itm->maxamt = 64;
	itm->amt = 1;
	
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\glassbottle.png","GlassBottleTexture");
}

#endif // !rockitemitem_HPP
