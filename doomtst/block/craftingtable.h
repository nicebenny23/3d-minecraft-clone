#include "block.h"
#include "../items/menu.h"
#include "../util//vector2.h"
#ifndef tableblock_HPP
#define tableblock_HPP
struct craftingmenu :menu {

	recipemanager blkcont;
	
	craftingmenu(v2::Vector2 size) {
		menubox = newbox("images\\menutex.png", size, v2::zerov, 11);

		menubox->shouldrender = false;
		menutype = normalmenu;
	
		blkcont = recipemanager("3x3craft.txt", 3, 3);
		enabled = false;

	}
	void custominit() {

	}
	void save() {

	
		
	}
	void customopen() {

		
		blkcont.enable();
	}
	void customclose() {

		blkcont.disable();
		
	}
	void testclick() {


		if (enabled)
		{
		
			blkcont.updatestate();
		}

	}

	craftingmenu() = default;



};
struct craftingtablecomp :gameobject::component {
	
	craftingmenu men;
	void onplayerclick() {
		if (userinput::getinputkey('p').pressed)
		{
			men.open();
		}
		
		int l = 1;
	}
	void update() {

		men.testclick();
		if (userinput::getinputkey('j').pressed)
		{
			men.close();
		}
	}
	void ondestroy(){
		men.blkcont.resourcecontainer->writetofile();

		men.blkcont.newitemlocation->writetofile();
	}
	void start() {

		men = craftingmenu(v2::Vector2(3, 3));
	}
};
inline void tableinit(blockname::block* blk) {

	blk->mesh.setfaces(craftingtableside, craftingtableside,craftingtabletop, craftingtableside, craftingtableside, craftingtableside);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mesh.scale = blockname::unitscale;
	blk->createaabb();
	if (!blk->hascomponent<craftingtablecomp>())
	{

		blk->addcomponent<craftingtablecomp>();

	}
	blk->addcomponentptr<loottable>()->addelem(craftingtableitem, 1, false);
}
inline void tabledelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<craftingtablecomp>();

	blk->removecomponent<loottable>();
}
#endif // !wood_HPP

