#include "block.h"
#include "../items/menu.h"
#include "../util//vector2.h"
#ifndef tableblock_HPP
#define tableblock_HPP
struct craftingmenu :menu {

	recipemanager blkcont;
	
	craftingmenu(v2::Vector2 size) {
		menubox = ui::createuielement<uibox>("images\\menutex.png", size, v2::zerov, 11);

		menubox->state.enabled = false;
		menutype = normalmenu;
	
		blkcont = recipemanager("crafting\\3x3craft.txt", 3, 3);
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
		if (owner->state==gameobject::active)
		{
			if (userinput::mouseright.pressed)
			{
				men.open();
			}
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
		if (owner->state == gameobject::beingroughdestroyed)
		{
			men.blkcont.save();
		}
		men.blkcont.destroy();
	}
	void start() {

		men = craftingmenu(v2::Vector2(3, 3));
	}
};
inline void tableinit(blockname::block* blk) {

	blk->mesh.setfaces(craftingtableside, craftingtableside,craftingtabletop, craftingtableside, craftingtableside, craftingtableside);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mininglevel = 1;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	//stupid thing
	if (!blk->hascomponent<craftingtablecomp>())
	{

		blk->addcomponent<craftingtablecomp>();

	}
	blk->addcomponent<loottable>()->addelem(craftingtableitem, 1, false);
}
inline void tabledelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<craftingtablecomp>();
	
	blk->removecomponent<loottable>();
}
#endif // !wood_HPP

