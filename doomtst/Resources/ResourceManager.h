#pragma once
#include "../util/type_index.h"
#include "../util/dynamicarray.h"
//Control Resource deletion and loading
struct ResourceManager
{
	
	Cont::array<void*> resources;
	type_id::type_indexer resources_ids;
};