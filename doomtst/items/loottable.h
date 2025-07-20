#include "../game/gameobject.h"
#include "../util/dynamicarray.h"

#include "../util/random.h"

using namespace stn;
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
	void drop();
	

	lootelement(int itemid, float maxamt,bool happenrandom)
		: itemid(itemid), maxamt(maxamt)
	{
	
	}
	~lootelement() = default;
	
};
struct  loottable : gameobject::component
{
	
	bool should_drop;
	loottable() {

		should_drop = false;
		lootlist = array<lootelement>();
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
	void destroy_hook() {
		
	
		if (should_drop)
		{
			for (int i = 0; i < lootlist.length; i++)
			{
				lootlist[i].drop();
			}
		}
		lootlist.destroy();
	}
};



 // !loottable_HPP
