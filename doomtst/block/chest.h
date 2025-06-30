#include "block.h"
#include "../items/menu.h"
#include "../util/vector2.h"
#include "../items/recipe.h"
#pragma once 
struct chestmenu :menu {

	Container blkcont;

	chestmenu(v2::Vector2 size) {
		menubox = ui::createuielement<uibox>("images\\menutex.png", "MenuTexture", size, v2::zerov, 11);

		menubox->state.enabled = false;
		menutype = normalmenu;

		blkcont= Container(3,3,0,3);
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

			blkcont.update();
		}

	}

	chestmenu() = default;



};
struct chestcomp :gameobject::component {

	chestmenu men;
	void onplayerclick() {
		if (CtxName::ctx.Inp->mouseright().pressed) {
			men.open();
		}
		

		
	}
	void update() {

		men.testclick();
		if (CtxName::ctx.Inp->getKey('j').pressed)
		{
			men.close();
		}
	}
	void ondestroy() {
		men.blkcont.writetofile();
}
	void start() {

		men = chestmenu(v2::Vector2(3, 3));
	}
};
inline void chestinit(blockname::block* blk) {

	blk->mesh.setfaces(chestside, chestside, chestside, chestside, chestfront, chestside);
	blk->attributes.solid = true;
	blk->attributes.transparent = false;
	blk->emitedlight = 0;
	blk->mininglevel = 1;
	blk->mesh.box.scale = blockname::blockscale;

	blk->createdefaultaabb(false);
	if (!blk->hascomponent<chestcomp>())
	{

		blk->addcomponent<chestcomp>();

	}
blk->addcomponent < loottable > ()->addelem(chestitem, 1, false);
}
inline void chestdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<chestcomp>();

	blk->removecomponent<loottable>();
}
 // !wood_HPP

