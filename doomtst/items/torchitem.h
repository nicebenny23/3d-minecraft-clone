#include "item.h"
#include "../block/block.h"
#ifndef torchitem_HPP
#define torchitem_HPP
#define torchitemblockid blockname::minecraftlava
inline void inittorchitem(item* itm) {
	itm->properties.pickaxepower = 1;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->itemui.itemsprite= createitembox("images\\torch.png");
	itm->properties.placeable = true;
}

#endif // !torchitem_HPP
