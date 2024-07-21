#include "item.h"
#ifndef lavapickitem_HPP
#define lavapickitem_HPP
#define lavapickblockid -1
inline void lavapickinit(item* itm) {
	itm->pickaxepower = 5;
	itm->placeable = false;
	itm->itemsprite= uirender::newbox("images\\pikaxe.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
}

#endif // !lavapickitem_HPP
