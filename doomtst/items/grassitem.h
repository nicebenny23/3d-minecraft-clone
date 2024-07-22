#include "item.h"
#ifndef grassitem_HPP
#define grassitem_HPP
#define grassitemblockid 2
inline void initgrassitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 3;
	itm->placeable = true;
	itm->itemui.itemsprite = uirender::newbox("images\\grass.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, 100);
}
#endif // !grassitem_HPP
