#include "item.h"
#ifndef slimeitem_HPP
#define slimeitem_HPP
#define slimeitemasblock -1
inline void initslimeitem(item* itm) {

	itm->pickaxepower = 0;
	itm->maxamt = 64;
	itm->dmg = 0;
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\slime.png");

}

#endif // !slime_HPP
