#include "../game/ecs/game_object.h"
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
struct  loottable : ecs::component
{
	
	bool should_drop;
	loottable() {

		should_drop = false;
		lootlist = array<lootelement>();
	}
	void start() {
		
		
	}
	~loottable()
	{


	};
	array<lootelement> lootlist;
	void addelem(int itemid, float maxamt,bool israndom=false) {
		lootlist.push(lootelement(itemid, maxamt,israndom));
	}
	void destroy_hook() {
		
	
		if (should_drop)
		{
			for (int i = 0; i < lootlist.length(); i++)
			{
				lootlist[i].drop();
			}
		}
	}
};
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<loottable> = {
		.updates = false
	};
}



 // !loottable_HPP
