#include "../block/block.h"
#include "item.h"
#include "sworditem.h"
#include "lavapickitem.h"
#include "stoneitem.h"
#include "airitem.h"
#include "torchitem.h"
#include "treestoneitem.h"
#include "crystalitem.h"
#include "craftingtableitem.h"
#include "metalrod.h"
#include "crystalpickinit.h"
#include "item.h"
#include "slimeitem.h"
#include "crystaltorchitem.h"
#include "mossitem.h"
#include "rope.h"
#include "chestitem.h"
#include "furnaceitem.h"
#include "ironitem.h"
#include "ironoreitem.h"
#include "ironpick.h"
#include "ironsword.h"
#include "crystaldagger.h"
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
	case treestoneitem:
		inittreestoneitem(itm);
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
	case crystaltorchitem:
		initcrystaltorchitem(itm);
		break;
	case mossitem:
		initmossitem(itm);
		break;
	case ropeitem:
		initropeitem(itm);
		break;
	case chestitem:
		initchestitem(itm);
		break;
	case furnaceitem:
		initfurnaceitem(itm);
		break;

	case ironoreitem:
		(itm);
		break;
	case ironitem:
		initironitem(itm);
		break;
	case ironpickitem:
		ironpickinit(itm);
		break;
	case ironsworditem:
		initironsworditem(itm);
		break;
	case crystaldaggeritem:
		initcrystaldaggeritem(itm);
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

		inititemproperties(newitem);
		newitem->amt = amt;
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
	case treestoneitem:
		return treestoneitemblockid;
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
	case crystaltorchitem:
		return crystaltorchitemblockid;
		break;
	case mossitem:
		return mossitemblockid;
		break;
	case ropeitem:
		return ropeitemasblock;
		break;

	case chestitem:
		return chestblockid;
		break;
	case furnaceitem:
		return furnaceblockid;
		break;
	case ironoreitem:
		return ironoreitemasblockid;
		break;
	case ironitem:
		return ironitemasblockid;
		break;
	case ironpickitem:
		return ironpickblockid;
		break;
	case ironsworditem:
		return ironsworditemblockid;

		break;
	case crystaldaggeritem:
		return crystaldaggeritemasblockid;
		break;
	default:
		break;
	}

}
inline void getitemfromblock() {








}
#endif // !itemutil_HPP
