#include "../game/gameobject.h"
#include "../util/dynamicarray.h"
#include "../player/playerinventory.h"
#include "../util/random.h"
#include "../player/player.h"
using namespace Cont;
#pragma once 
const float interacttimeneededfordrop = 1;
struct lootelement
{
	
	
	lootelement() {
		itemid = 0;
		maxamt = 0;
	
	}
	unsigned char itemid;
	unsigned char maxamt;
	void drop() {
		int dropamt = maxamt;
player::goblin.getcomponent<inventory>().hotbar.fill(itemid, dropamt, false);
		player::goblin.getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt,false);
		player::goblin.getcomponent<inventory>().hotbar.fill(itemid, dropamt, true);

		player::goblin.getcomponent<inventory>().playermenu.blkcont.fill(itemid, dropamt, true);
	}

	lootelement(int itemid, float maxamt,bool happenrandom)
		: itemid(itemid), maxamt(maxamt)
	{
	
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
		lootlist = array<lootelement>(1);
		utype = gameobject::updatenone;
	}
	void start() {
		
		
	}
	~loottable()
	{

		if (lootlist.list!=nullptr)
		{
			Assert("lootlist not nullptr");
		}
	};
	array<lootelement> lootlist;
	void addelem(int itemid, float maxamt,bool israndom=false) {
		lootlist.push(lootelement(itemid, maxamt,israndom));
	}
	void ondestroy() {
		
	
		if (!playerinteract)
		{
			lootlist.destroy();
			return;
		}
		if (owner.hascomponent<blockname::block>())
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



 // !loottable_HPP
