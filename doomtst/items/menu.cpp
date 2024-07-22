#include "menu.h"

menu* openmenu;
menu* inventorylocation;
bool ismenuopen() {
	if (openmenu!=nullptr)
	{
		return true;
	}if (inventorylocation->isopen)
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
		menubox->shouldrender = false;
		customclose();
		return;
	}

	isopen = false;
	openmenu = nullptr;
	menubox->shouldrender=false;
	customclose();
	inventorylocation->close();
}

void menu::open()
{
	if (menutype==inventorymenu)
	{


		menubox->shouldrender = true;
		isopen = true;
		customopen();
		return;
	}
	
	
	if (isopen)
	{
		return;
	}



	if (openmenu!=nullptr)
	{
			openmenu->close();
	}
	
	inventorylocation->open();
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
