#pragma once
#include "gameobject.h"
#include "../block/block.h"
#include "entity.h"

// Returns center of object
namespace objutil {

	inline blockname::block* toblock(gameobject::obj& a) {
		return (a.type() == gameobject::block) ? (blockname::block*)(&a) : nullptr;
	}


	inline blockname::block& toblk(gameobject::obj* object) {
		return (object)->getcomponent<blockname::block>();
	}

	inline blockname::block& toblk(gameobject::obj& object) {
		return object.getcomponent<blockname::block>();
	}

	

	
}
