#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include "../player/playerinventory.h"
#include "../util/random.h"
#include "../player/player.h"
using namespace dynamicarray;
#ifndef loottable_HPP
#define loottable_HPP

struct lootelement
{
	lootelement() {
		itemid = -1;
		maxamt = -1;
	}
	int itemid;
	float maxamt;
	void drop() {

		int dropamt=2*random(maxamt);

		player::goblin.toent()->getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt);
	}

	lootelement(int itemid, float maxamt)
		: itemid(itemid), maxamt(maxamt)
	{
	}
	~lootelement() = default;
};
struct  loottable :gameobject::component
{

	void start() {

		lootlist =  array<lootelement>();
	}
	~loottable()
	{

		if (lootlist.getdata()!=nullptr)
		{
			Assert("lootlist not nullptr");
		}
	};
	array<lootelement> lootlist;
	void addelem(int itemid, float maxamt) {
		lootlist.append(lootelement(itemid, maxamt));
	}
	void ondestroy() {
		if (owner->type==gameobject::block)
		{
			if (grid::getobjatgrid(toblk(owner).pos)==nullptr)
			{
				lootlist.destroy();
				return;
			}
		}
	
		for (int i = 0; i < lootlist.length; i++)
		{
			lootlist[i].drop();
			lootlist[i];
		}
		lootlist.destroy();
}
};



#endif // !loottable_HPP
