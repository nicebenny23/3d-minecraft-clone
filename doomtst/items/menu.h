#include "../renderer/uirender.h"
#include "inventoryblock.h"

using namespace uirender;
using namespace dynamicarray;
#ifndef  menu_HPP
#define menu_HPP
enum type {
	inventorymenu=1,
	normalmenu=2,

};
bool ismenuopen();
struct menu
{
	
	void close();
	void(*closeinven);
		uibox* menubox;
		type menutype;
		
	bool isopen;
	void open();
	virtual void customclose();

	virtual void customopen();
	menu() {
		menubox = nullptr;
		closeinven = nullptr;
	};
	menu(v2::Vector2 size);
	virtual void testclick();
	virtual void custominit();
};

extern menu* openmenu;
extern menu* inventorylocation;
#endif // ! menu_HPP
