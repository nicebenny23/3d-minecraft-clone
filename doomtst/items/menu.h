#include "../renderer/uibox.h"


using namespace uiboxname;
using namespace stn;
#pragma once 
enum type {
	inventorymenu=1,
	normalmenu=2,
	settingsmenu=3,
};
bool ismenuopen();
struct menu
{
	
	void close();
	void(*closeinven);
	Cptr::cptr<uibox> menubox;
		type menutype;
		
	bool enabled;
	void open();
	virtual void customclose();

	virtual void customopen();
	menu() {
		menubox = Cptr::cptr<uibox>( nullptr);
		closeinven = nullptr;
	};
	menu(v2::Vec2 size);
	virtual void testclick();
	virtual void custominit();
};
;
extern menu* openmenu;
extern menu* inventorylocation;
void managemenus();
 // ! menu_HPP
