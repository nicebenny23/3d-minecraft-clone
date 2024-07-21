#include "menu.h"

menu* openmenu;
menu* inventoryifopen;
bool ismenuopen() {
	if (openmenu!=nullptr)
	{
		return true;
	}if (inventoryifopen!=nullptr)
	{
		return true;
	}
	return false;
}
void menu::close()
{
	if (menutype==inventorymenu)
	{

		isopen = false;
		inventoryifopen= nullptr;
		menubox->shouldrender = false;
		customclose();
	}
	isopen = false;
	openmenu = nullptr;
	menubox->shouldrender=false;
	customclose();

}

void menu::open()
{
	if (menutype==inventorymenu)
	{

		if (inventoryifopen!=nullptr)
		{
			return;
		}
		inventoryifopen = this;
		menubox->shouldrender = true;
		isopen = true;
	}
	if (isopen)
	{
		return;
	}
	if (openmenu!=nullptr)
	{
			openmenu->close();
	}
	
			
			menubox->shouldrender = true;
			isopen = true;
			openmenu = this;
			customopen();
}

void menu::customclose()
{
	
}

void menu::customopen()
{

}

menu::menu(v2::Vector2 size)
{

	menubox = newbox("images\\menutex.png", size,v2::zerov,11);
	menubox->shouldrender = false;
	
}
 void menu::testclick()
{

	
}

 void menu::custominit()
 {
	 int l = 1;

 }
