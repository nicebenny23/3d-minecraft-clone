#include "item.h"
#ifndef torchitem_HPP
#define torchitem_HPP
#define torchitemblockid 7
inline void inittorchitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 3;
	itm->itemsprite= uirender::newbox("images\\torch.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
	itm->placeable = true;
}

#endif // !torchitem_HPP
