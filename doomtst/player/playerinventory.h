#include "../items/menu.h"
#include "../game/gameobject.h"
#include "../util/userinput.h"
#ifndef playerinventory_Hpp
#define playerinventory_Hpp
inline void initfreeditem() {

	freeditem = inititem(0);
	freeditem->state = beingheld;
	
}
struct inventory :gameobject::component
{
	
	menu playermenu;
	inventory();
		Container hotbar;
		item* selected;
	void update() {
		
		hotbar.testmouseclick();
		if (userinput::getinputkey('j').pressed) {
			playermenu.close();
		}

		if (userinput::getinputkey('e').pressed) {
			playermenu.open();
		}
		if (userinput::getinputkey('1').pressed)
		{
			selected = hotbar.databuf[0].helditem;
		}
		if (userinput::getinputkey('2').pressed)
		{
			selected = hotbar.databuf[1].helditem;
		}
		if (userinput::getinputkey('3').pressed)
		{
			selected = hotbar.databuf[2].helditem;
		}if (userinput::getinputkey('4').pressed)
		{
			selected = hotbar.databuf[3].helditem;
		}
		if (userinput::getinputkey('5').pressed)
		{
			selected = hotbar.databuf[4].helditem;
		}
		if (userinput::getinputkey('6').pressed)
		{
			selected = hotbar.databuf[5].helditem;

		}
		if (freeditem!=nullptr)
		{

			freeditem->itemsprite->box.center = userinput::normedmousepos;

		}
		playermenu.testclick();
		playermenu.blkcont.deletebelowzero();
	}
};



#endif // !playerinventory_H
