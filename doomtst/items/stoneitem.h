#include "item.h"
#ifndef stoneitem_HPP
#define stoneitem_HPP
#define stoneitemasblockid 3 

inline void initstoneitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 3;

	itm->placeable = true;
	itm->itemui.itemsprite= uirender::newbox("images\\stone.png", v2::unitv/30.f, v2::zerov, 100);
}


#endif // !stoneitem_HPP
