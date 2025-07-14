#include "gameobject.h"
#include "../util/SparseSet.h"
#pragma once
struct Relation_Edge{
	Ids::Id start;
	Ids::Id end;
	size_t relation_id;
};
struct relationship :gameobject::component {


	Sparse::PackedSet<> outgoing;
	



};

struct relation_set {
	


};