#include "item.h"
#include "../block/block.h"
#ifndef torchitem_HPP
#define torchitem_HPP
#define torchitemblockid 7
inline void inittorchitem(item* itm) {
	itm->pickaxepower = 1;
	itm->maxamt = 64;

	itm->itemtype = count;
	itm->itemui.itemsprite= createitembox("images\\torch.png");
	itm->placeable = true;
}

#endif // !torchitem_HPP
