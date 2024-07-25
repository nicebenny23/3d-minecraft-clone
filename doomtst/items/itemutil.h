#include "../block/block.h"
#include "item.h"
#include "sworditem.h"
#include "lavapickitem.h"
#include "stoneitem.h"
#include "airitem.h"
#include "torchitem.h"
#include "dirtitem.h"
#include "crystalitem.h"
#include "craftingtableitem.h"
#include "metalrod.h"
#include "crystalpickinit.h"
#include "item.h"
#include "slimeitem.h"
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
	case dirtitem:
		initdirtitem(itm);
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
	case slimeballitem:
		initslimeitem(itm);
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
	case dirtitem:
		return dirtitemblockid;
		break;
	case torchitem:
		return torchitemblockid;
		break;
	case pickitemid:
		return lavapickblockid;
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
	case slimeballitem:
		return slimeitemasblock;
		break;
	default:
		break;
	}

}
inline void getitemfromblock() {








}
#endif // !itemutil_HPP
