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
	bool israndom;
	lootelement() {
		itemid = -1;
		maxamt = -1;
	}
	int itemid;
	float maxamt;
	void drop() {
		int dropamt = maxamt;
		if (israndom)
		{

			 dropamt = 2 * random(maxamt);

		}player::goblin.toent()->getcomponent<inventory>().hotbar.fill(itemid, dropamt);
		player::goblin.toent()->getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt);
	}

	lootelement(int itemid, float maxamt,bool happenrandom)
		: itemid(itemid), maxamt(maxamt)
	{
		israndom = happenrandom;
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
	void addelem(int itemid, float maxamt,bool israndom=false) {
		lootlist.append(lootelement(itemid, maxamt,israndom));
	}
	void ondestroy() {
		if (owner->type==gameobject::block)
		{

		
			if (owner->state == gameobject::beingroughdestroyed)
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
