#pragma once
#include "gameobject.h"
#include "../block/block.h"
#include "entity.h"

// Returns center of object
namespace objutil {

	inline blockname::block* toblock(gameobject::obj& a) {
		return (a.type == gameobject::block) ? (blockname::block*)(&a) : nullptr;
	}

	inline Ent::entity* toentity(gameobject::obj& a) {
		return (a.type == gameobject::entity) ? (Ent::entity*)(&a) : nullptr;
	}

	inline blockname::block& toblk(gameobject::obj* object) {
		return *((blockname::block*)(object));
	}

	inline blockname::block& toblk(gameobject::obj& object) {
		return *((blockname::block*)(&object));
	}

	inline Ent::entity& toent(gameobject::obj* object) {
		return *((Ent::entity*)(object));
	}

	inline Ent::entity& toent(gameobject::obj& object) {
		return *((Ent::entity*)(&object));
	}

	inline Vector3 getcenter(gameobject::obj& object) {
		if (object.type == gameobject::block) {
			return toblk(object).pos + unitv / 2;
		}
		else {
			return ((Ent::entity*)(&object))->transform.position;
		}
		
	}
}
