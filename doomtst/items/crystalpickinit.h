#include "item.h"
#ifndef crystalpickitem_HPP
#define crystalpickitem_HPP
#define crystalpickblockid -1
inline void crystalpickinit(item* itm) {
	itm->properties.pickaxepower = 7;
	itm->properties.placeable = false;
	itm->maxamt = 512;
	itm->amt = 256;
	itm->itemtype = wear;
	itm->itemui.itemsprite = createitembox("images\\crystalpick.png","CrystalPickaxeTexture");
}

#endif // !crystalpickitem_HPP
