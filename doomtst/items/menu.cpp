#include "menu.h"

menu* openmenu;

void menu::close()
{
	isopen = false;
	openmenu = nullptr;
	menubox->shouldrender=false;
	blkcont.setviewable(false);
}

void menu::open()
{

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
			blkcont.setviewable(true);
}

menu::menu(v2::Vector2 size) 
{
	blkcont = Container(4, 4,0,0);
	menubox = newbox("menutex.png", size,v2::zerov,-111);
	menubox->shouldrender = true;
}

void menu::testclick(item* held)
{
	for (size_t i = 0; i < blkcont.databuf.length; i++)
	{
		blkcont.databuf[i].testclick(held);
	}
}

void menu::onopen()
{
}

void menu::onclose()
{
}
