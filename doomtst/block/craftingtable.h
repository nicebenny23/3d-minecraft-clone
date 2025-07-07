#include "block.h"
#include "../items/menu.h"
#include "../util//vector2.h"
#pragma once 
struct craftingmenu :menu {

	recipemanager blkcont;
	
	craftingmenu(v2::Vector2 size) {
		menubox = ui::createuielement<uibox>("images\\menutex.png", "MenuTexture", size, v2::zerov, 11);

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
struct craftingtablecomp : gameobject::component {
	
	craftingmenu men;
	void onplayerclick() {
		if (owner.state() ==gameobject::active)
		{
			if (CtxName::ctx.Inp->mouseright().pressed)
			{
				men.open();
			}
		}
	
		
		int l = 1;
	}
	void update() {

		men.testclick();
		if (CtxName::ctx.Inp->getKey('j').pressed)
		{
			men.close();
		}
	}
	void ondestroy(){
		if (owner.state() == gameobject::destroying)
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
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	//stupid thing
	if (!blk->owner.hascomponent<craftingtablecomp>())
	{

		blk->owner.addcomponent<craftingtablecomp>();

	}
	blk->owner.addcomponent<loottable>()->addelem(craftingtableitem, 1, false);
}
inline void tabledelete(blockname::block* blk) {
	blk->owner.removecomponent<aabb::Collider>();

	blk->owner.removecomponent<craftingtablecomp>();
	
	blk->owner.removecomponent<loottable>();
}
 // !wood_HPP

