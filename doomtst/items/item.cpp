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

	if (amt < 0)
	{
		amt += useamt;
		return false;
	}
	return true;
}
bool item::canuse(int useamt)
{
	if (amt-useamt < 0) {

		return false;
	}
	return true;
}
void item::maxoutthis(item* itm)
{
	int numcanstore = maxamt - amt;
	if (numcanstore>0)
	{
		int amttaken = Min(numcanstore, itm->amt);
		itm->amt -= amttaken;
		amt += amttaken;
	}
}
item* freeditem;
texturearray itemidlist;
