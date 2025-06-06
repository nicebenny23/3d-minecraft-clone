#include "item.h"
#include "../block/block.h"
#ifndef altaritem_HPP
#define altaritem_HPP
#define altaritemblockid blockname::minecraftaltar
inline void initaltaritem(item* itm) {
	itm->properties.pickaxepower = 1;

	itm->maxamt = 1;
	itm->properties.placeable = true;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\crystalaltarside.png","CrystalAltarSideTexture");
}
#endif // !grassitem_HPP
