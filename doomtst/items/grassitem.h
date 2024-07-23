#include "item.h"
#ifndef grassitem_HPP
#define grassitem_HPP
#define grassitemblockid 2
inline void initgrassitem(item* itm) {
	itm->pickaxepower = 0;
	itm->maxamt = 3;
	itm->placeable = true;
	itm->itemui.itemsprite = createitembox("images\\grass.png");
}
#endif // !grassitem_HPP
