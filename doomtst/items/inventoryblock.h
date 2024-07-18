#include "item.h"
#include "../util/vector3.h"
#include "../renderer/uirender.h"
#include  "../util/userinput.h"
const int xamt=20;
const int yamt=30;
using namespace v3;
#ifndef invblock_HPP
#define invblock_HPP
struct invblock
{
	item held;
	bool empty;
	void hide();
	void show();
	//(0...9),(0...14)
	Coord invblocklocation;
	uirender::uibox* framebox;
	void(*onclick)(invblock&);
	invblock(int xloc, int yloc);
	invblock(int xloc, int yloc,void (*clickaction)(invblock&));
	void giveitem(int id);
	void transferitem(item* otherholder);
	void destroyitem();
};
array<invblock> createinvarray(int xsize,int ysize,int xoffset,int yoffset) {



}

#endif // !invblock_HPP
