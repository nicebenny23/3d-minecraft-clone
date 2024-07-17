#include "item.h"
#include "../util/random.h"
item::item(int itemid)
{
	id = randomint(3);
	
	maxamt = 1;
	if (id==sworditem)
	{
		itemsprite = uirender::newbox("slimetex.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, -101000);
	}
	else {
		itemsprite = uirender::newbox("pikaxe.png", v2::Vector2(1 / 20.f, 1 / 20.f), v2::zerov, -101000);
	     }
}
