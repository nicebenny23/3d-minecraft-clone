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

		int dropamt=114*random(maxamt);
		player::goblin.toent()->getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt);
	}

	lootelement(int itemid, float maxamt)
		: itemid(itemid), maxamt(maxamt)
	{
	}
};
struct  loottable:gameobject::component
{

	void start() {

		lootlist = *new array<lootelement>();
	}
	~loottable() = default;
	array<lootelement> lootlist;
	void addelem(int itemid, float maxamt) {
		lootlist.append(lootelement(itemid, maxamt));
	}
	void ondestroy() {

		for (int i = 0; i < lootlist.length; i++)
		{
			lootlist[i].drop();
		}
		lootlist.destroy();
}
};



#endif // !loottable_HPP