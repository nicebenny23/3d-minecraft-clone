#pragma once
#include "item.h"
#include "../util/random.h"

item::item(int itemid)
{


		id = itemid;

		properties.foodval = 0;
	itemui.itemsprite =Cptr::cptr< uiboxname::uibox>( nullptr);
	itemui.textvalue= ui::createuielement<integertext>(v2::zerov,1/80.0f);
	maxamt = 0;
	properties.pickaxepower = 1;
	properties.dmg = 1;
}
bool item::canadd(int amtto)
{
	int cap = maxamt-amt;

	if (amtto<=cap)
	{
		return true;
	}
	return false;
}
void item::destroy()
{
	itemui.textvalue->destroy();
	itemui.itemsprite->destroy();
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
void item::give(int& givenamt)
{
	int numcanstore = maxamt - amt;
	if (numcanstore > 0)
	{
		int amttaken = Min(numcanstore, givenamt);
		givenamt-= amttaken;
		amt += amttaken;
	}
}
void item::maxoutthis(item* itm)
{
	this->give(itm->amt);
}
item* freeditem;