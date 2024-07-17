#include "../items/menu.h"
#include "../game/gameobject.h"
#include "../util/userinput.h"
#ifndef playerinventory_Hpp
#define playerinventory_Hpp
struct inventory :gameobject::component
{
	item* playerheld;
	menu playermenu;
	inventory();

	void update() {
		
		if (userinput::getinputkey('j').pressed) {
			playermenu.close();
		}

		if (userinput::getinputkey('e').pressed) {
			playermenu.open();
		}
		playerheld->itemsprite->bx.center = userinput::normedmousepos;
		
		playermenu.testclick(playerheld);
	}
};



#endif // !playerinventory_H
