#include "../block/block.h"
#include "item.h"
#include "sworditem.h"
#include "lavapickitem.h"
#include "stoneitem.h"
#include "airitem.h"
#include "torchitem.h"
#include "grassitem.h"
#include "crystalitem.h"
#include "craftingtableitem.h"
#include "metalrod.h"
#include "crystalpickinit.h"
#ifndef itemutil_HPP
#define itemutil_HPP
inline void inititemproperties(item* itm) {

	switch (itm->id)
	{

	case stoneitem:
		initstoneitem(itm);
		break;
	case sworditem:

		initsworditem(itm);
		break;
	case grassitem:
		initgrassitem(itm);
		break;
	case torchitem:
		inittorchitem(itm);
		break;
	case pickitemid:
		lavapickinit(itm);
		break;
	case crystalitem:
		initcrystalitem(itm);
		break;
	case metalroditem:
		initmetalroditem(itm);
		break;
	case craftingtableitem:
		initcraftingtable(itm);
		break;
	case crystalpickitemid:
	crystalpickinit	(itm);
		break;
	default:
	//	Assert("null item not initiated");
		break;
	}


}
inline item* inititem(int id) {
	if (id==0)
	{
		return nullptr;
	}
	int rint = id;

	item* newitem = new item(rint);
	inititemproperties(newitem);
	return newitem;
}

	inline item* inititem(int id,int amt) {
		if (id==0)
		{

			return nullptr;
		}
		int rint = id;
		item* newitem = new item(rint);
		newitem->amt = amt;
		inititemproperties(newitem);
		return newitem;

	}

inline int blockidfromitemid(item* itm) {

	switch (itm->id)
	{

	case stoneitem:
		return stoneitemasblockid;
		break;
	case sworditem:
		return sworditemblockid;

		break;
	case grassitem:
		return grassitemblockid;
		break;
	case torchitem:
		return torchitemblockid;
		break;
	case pickitemid:
		return pickitemid;
		break;
	case crystalitem:
		return crystalitemasblockid;
		break;
	case metalroditem:
		return metalroditemasblockid;
		break;
	case craftingtableitem:
		return craftingtableblockid;
		break;
	case crystalpickitemid:
		return crystalitemasblockid;
		break;
	default:
		break;
	}

}
inline void getitemfromblock() {








}
#endif // !itemutil_HPP
