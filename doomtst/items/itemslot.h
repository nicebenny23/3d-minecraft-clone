#include "item.h"
#include "../util/vector3.h"
#include "../renderer/uirender.h"
#include  "../util/userinput.h"
#include "itemutil.h"
using namespace v3;
#ifndef invblock_HPP
#define invblock_HPP
const int xamt = 20;
const int yamt = 30;
struct itemslot
{
	itemslot() {
		Assert("invblockconstrutur cant be used just to get complier to work");

	}
	item* helditem;

	//(0...9),(0...14)
	Coord location;
	uirender::uibox* frame;
	void(*onclick)(itemslot&);
	
	itemslot(int xloc, int yloc);
	itemslot(int xloc, int yloc, void (*clickaction)(itemslot&));
	void giveitem(int id);
	void giveitem(int id,int amt);
	void transferitem(item* otherholder);
	void destroyitem();
	void enable();
	void disable();
	bool hasbeenclicked();
	void updatestate();
};

#endif // !invblock_HPP
