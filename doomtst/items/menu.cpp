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

void menu::customclose()
{
}

void menu::customopen()
{
}

menu::menu(v2::Vector2 size)
{
	blkcont = Container(8, 4,0,0);
	menubox = newbox("menutex.png", size,v2::zerov,11);
	menubox->shouldrender = true;
}

void menu::testclick()
{

	if (isopen)
	{

		blkcont.testmouseclick();
	}
}

void menu::onopen()
{
}

void menu::onclose()
{
}
