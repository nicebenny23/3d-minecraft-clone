#include "block.h"
#include "../items/menu.h"
#include "../util/vector2.h"
#include "../items/recipe.h"
#ifndef chestblock_HPP
#define chestblock_HPP
struct chestmenu :menu {

	Container blkcont;

	chestmenu(v2::Vector2 size) {
		menubox = newbox("images\\menutex.png", size, v2::zerov, 11);

		menubox->shouldrender = false;
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
		if (userinput::getinputkey('p').pressed)
		{
			men.open();
		}

		
	}
	void update() {

		men.testclick();
		if (userinput::getinputkey('j').pressed)
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

	blk->mesh.setfaces(chestside, chestside, chesttop, chestside, chestside, chestside);
	blk->solid = true;
	blk->transparent = false;
	blk->emitedlight = 0;
	blk->mininglevel = 1;
	blk->mesh.scale = blockname::unitscale;

	blk->createaabb(false);
	if (!blk->hascomponent<chestcomp>())
	{

		blk->addcomponent<chestcomp>();

	}
//blk->addcomponentptr < loottable > ()->addelem(chestitem, 1, false);
}
inline void chestdelete(blockname::block* blk) {
	blk->removecomponent<aabb::Collider>();

	blk->removecomponent<chestcomp>();

	//blk->removecomponent<loottable>();
}
#endif // !wood_HPP

