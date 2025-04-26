#include "item.h"
#ifndef slimeitem_HPP
#define slimeitem_HPP
#define slimeitemasblock -1
inline void initslimeitem(item* itm) {

	itm->properties.pickaxepower = 0;
	itm->maxamt = 64;
	itm->properties.dmg = 0;
	itm->itemtype = count;
	itm->itemui.itemsprite = createitembox("images\\slime.png","SlimeTexture");

}

#endif // !slime_HPP
