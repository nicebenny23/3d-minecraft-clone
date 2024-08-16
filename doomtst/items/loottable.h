#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include "../player/playerinventory.h"
#include "../util/random.h"
#include "../player/player.h"
using namespace dynamicarray;
#ifndef loottable_HPP
#define loottable_HPP
const float interacttimeneededfordrop = 1;
struct lootelement
{
	
	bool israndom;
	lootelement() {
		itemid = 0;
		maxamt = 0;
		israndom = false;
	}
	byte itemid;
	char maxamt;
	void drop() {
		int dropamt = maxamt;
		if (israndom)
		{

			 dropamt = 2 * random(maxamt);

		}player::goblin.toent()->getcomponent<inventory>().hotbar.fill(itemid, dropamt, false);
		player::goblin.toent()->getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt,false);
		player::goblin.toent()->getcomponent<inventory>().hotbar.fill(itemid, dropamt, true);

		player::goblin.toent()->getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt, true);
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

	bool playerinteract;
	void onplayerclick() {
		playerinteract = true;
		
	}
	loottable() {

		playerinteract = false;
		lootlist = array<lootelement>(1, false);
		utype = gameobject::updatenone;
	}
	void start() {
		
		
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
		if (owner->state == gameobject::beingroughdestroyed)
		{
			lootlist.destroy();
			return;
		}
	
		if (owner->type==gameobject::entity)
		{
			if (!playerinteract) {
				return;
			}
		}
		if (owner->type == gameobject::block)
		{
			if (objutil::toblk(owner).bstate.broken != true)
			{
				lootlist.destroy();
			}
		}
		for (int i = 0; i < lootlist.length; i++)
		{
			
			lootlist[i].drop();
		
		}
		lootlist.destroy();
}
};



#endif // !loottable_HPP
