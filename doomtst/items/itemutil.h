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
#include "ironbottomitem.h"
#include "crystalbottom.h"
#include "crystaltop.h"
#include "irontopitem.h"
#include "ironsword.h"
#include "crystaldagger.h"
#include "plank.h"
#include "altaritem.h"
#include "obsidianitem.h"
#include "glassitem.h"
#include "glassbottle.h"
#include "sand.h"
#include "tpsword.h"
#pragma once 
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
		initironoreitem(itm);
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
	case irontop:
		initirontopitem(itm);
		break;
	case ironbottom:
		initironbottomitem (itm);
		break;
	case crystaltop:
		initcrystaltopitem(itm);
		break;
	case crystalbottom:
		initcrystalbottomitem(itm);
		break;
	case plankitem:
		initplankitem(itm);
		break;
	case altaritem:
		initaltaritem(itm);
		break;
	case obsidianitem:
		initobsidianitem(itm);
		break;
	case sanditem:
		sanditeminit(itm);
		break;
	case glassitem:
		glassiteminit(itm);
		break;
	case glassbottleitem:

		glassbottleiteminit(itm);
		break;
	case playertpsword:
		initfinalsworditem(itm);
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
	case irontop:
		return irontopitemblockid;

		break;
	case ironbottom:
		return ironbottomitemblockid;
		break;
	case crystaltop:
		return crystaltopitemblockid;
		break;
	case crystalbottom:
		return crystalbottomitemblockid;
		break;
	case plankitem:
		return plankitemblockid;
		break;
	case obsidianitem:
		return obsidianasblockid;
		break;
	case altaritem:
		return altaritemblockid;
		break;
	case sanditem:
		return sanditemblockid;
     break;
	case glassitem:
		return glassitemblockid;
		break;
	case glassbottleitem:
		return glassbottleitemblockid;
		break;

	case playertpsword:
		return tpswordblockid;
	default:

		break;
	}

}
inline void getitemfromblock() {








}
 // !itemutil_HPP
