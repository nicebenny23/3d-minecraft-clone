#include "block.h"
#include "../items/menu.h"
#include "../math/vector2.h"
#include "../items/recipe.h"
#pragma once 
struct chestmenu :menu {

	Container blkcont;

	chestmenu(v2::Vec2 size):menu(size){
		
		menubox->disable();
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
struct chestcomp : ecs::component{

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
	void destroy_hook() {
		men.blkcont.writetofile();
	}

	chestcomp():men(v2::Vec2(3, 3)){
	}
};
inline void chestinit(blockname::block& blk) {

	blk.mesh.setfaces(chestside, chestside, chestside, chestside, chestfront, chestside);
	blk.attributes.solid = true;
	blk.attributes.transparent = false;
	blk.emitedlight = 0;
	blk.mininglevel = 1;
	blk.mesh.box.scale = blockname::blockscale;

	blk.createdefaultaabb(false);
	if (!blk.owner().has_component<chestcomp>())
	{

		blk.owner().add_component<chestcomp>();

	}
blk.owner().add_component < loottable > ().addelem(chestitem, 1, false);
}
