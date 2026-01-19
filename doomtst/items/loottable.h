#include "../game/ecs/game_object.h"
#include "../util/dynamicarray.h"
#include"item_transactions.h"
#include "../util/random.h"

using namespace stn;
#pragma once 
const float interacttimeneededfordrop = 1;
struct lootelement
{
	
	
	lootelement() = default;
	std::string item_name;
	unsigned char maxamt;
	void drop(ecs::Ecs& world);
	

	lootelement(std::string name, float maxamt,bool happenrandom)
		: item_name(name), maxamt(maxamt)
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
	void addelem(std::string item_name, float maxamt,bool israndom=false) {
		lootlist.push(lootelement(item_name, maxamt,israndom));
	}
	void destroy_hook() {
		
	
		if (should_drop)
		{
			for (int i = 0; i < lootlist.length(); i++)
			{
				lootlist[i].drop(world());
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
