#include "item.h"
#include "../util/vector3.h"
#include "../renderer/uirender.h"
#include  "../util/userinput.h"

using namespace v3;
#ifndef invblock_HPP
#define invblock_HPP
const int xamt = 20;
const int yamt = 30;
struct invblock
{
	invblock() {
		Assert("invblockconstrutur cant be used just to get complier to work");
	
	}
	item held;
	bool empty;
	//(0...9),(0...14)
	Coord invblocklocation;
	uirender::uibox* framebox;
	void(*onclick)(invblock&);
	invblock(int xloc, int yloc);
	invblock(int xloc, int yloc,void (*clickaction)(invblock&));
	void giveitem(int id);
	void transferitem(item* otherholder);
	void destroyitem();
	void setviewable(bool isviewable);

	void testclick(item* helditem);
};

array<invblock>& createinvblock(int xsize, int ysize);
#endif // !invblock_HPP
