#include "../block/block.h"
#include "gameobject.h"
#include "entity.h"

#ifndef gamehelper_hpp
#define gamehelper_hpp
//work on ptrs

//returns center of object
namespace objutil {
#define toblock(a)(a.type==gameobject::block?(blockname::block*)(&a):nullptr)

#define toentity(a)((a.type==gameobject::entity)?(entity::entity*)(&a):nullptr)
	inline blockname::block& toblk(gameobject::obj* object) {

		return *((blockname::block*)(object));
	}
	inline blockname::block& toblk(gameobject::obj& object) {

		return *((blockname::block*)(&object));
	}inline entityname::entity& toent(gameobject::obj* object) {

		return *((entityname::entity*)(object));
	}
	inline entityname::entity& toent(gameobject::obj& object) {

		return *((entityname::entity*)(&object));
	}



	inline Vector3 getcenter(gameobject::obj& object) {

	if (object.type==gameobject::block)
	{
		toblk(object).pos+ unitv / 2;
	}
	if (object.type==gameobject::entity)
	{
		((entityname::entity*)(&object))->transform.position;
	}
}
//returns id ob object only works for block


}
#endif // !gamehelper_Hpp

