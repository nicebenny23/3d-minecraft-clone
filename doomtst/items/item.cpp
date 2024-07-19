#include "item.h"
#include "../util/random.h"
item::item(int itemid)
{
	id = itemid;
	itemsprite = nullptr;

	
}
void item::destroy()
{
	itemsprite->destroy();
}
bool item::use(int useamt)
{
	amt -= useamt;
	
	if (amt<0)
	{
		amt += useamt;
		return false;
	}
	return true;
}
item* freeditem;
texturearray itemidlist;
