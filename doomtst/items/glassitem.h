#include "item.h"
#include "../block/block.h"
#ifndef glassitem_HPP
#define glassitem_HPP
#define glassitemblockid blockname::minecraftglass
inline void glassiteminit(item* itm) {
	itm->properties.pickaxepower = 2;
	itm->properties.placeable = true;
	itm->maxamt = 64;
	itm->amt = 0;
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\glass.png","GlassTexture");
}

#endif // !rockitemitem_HPP
