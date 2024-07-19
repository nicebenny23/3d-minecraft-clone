#include "../block/block.h"
#include "item.h"
#include "sworditem.h"
#include "lavapickitem.h"
#include "stoneitem.h"
#include "airitem.h"
#include "torchitem.h"
#include "grassitem.h"
#ifndef itemutil_HPP
#define itemutil_HPP
inline void placeitemid(item* itm) {

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
	default:
		break;
	}


}
inline item* inititem(int id) {
	int rint = randomint(5);
	item* newitem = new item(rint);
	placeitemid(newitem);
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
	default:
		break;
	}
	
}
inline void getitemfromblock() {








}
#endif // !itemutil_HPP
