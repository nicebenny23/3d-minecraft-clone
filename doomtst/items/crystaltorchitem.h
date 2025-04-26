#include "item.h"
#include "../block/block.h"
#ifndef crystaltorchitem_HPP
#define crystaltorchitem_HPP
#define crystaltorchitemblockid 10
inline void initcrystaltorchitem(item* itm) {
	itm->properties.pickaxepower = 1;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\crystaltorch.png","CrystalTorchTexture");
	itm->properties.placeable = true;
}

#endif // !torchitem_HPP
