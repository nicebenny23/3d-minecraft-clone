#include "item.h"
#ifndef sworditem_HPP
#define sworditem_HPP
#define sworditemblockid -1
inline void initsworditem(item* itm) {
	itm->pickaxepower = 0;
	itm->placeable = false;
	itm->maxamt = 3;
itm->itemui.itemsprite = uirender::newbox("images\\sword.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
}
#endif // !sworditem_HPP
