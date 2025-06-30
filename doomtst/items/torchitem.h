#include "item.h"
#include "../block/block.h"
#pragma once 
#define torchitemblockid blockname::minecrafttorch
inline void inittorchitem(item* itm) {
	itm->properties.pickaxepower = 1;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->itemui.itemsprite= createitembox("images\\torch.png","TorchTexture");
	itm->properties.placeable = true;
}

 // !torchitem_HPP
