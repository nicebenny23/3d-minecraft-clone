#include "system.h"
#pragma once
namespace ecs {

	
	template<ecs::SystemType T> 
	struct BuiltSystem:ecs::System{
		using args = stn::arguments_t<T>;	
	};

}