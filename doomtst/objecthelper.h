#include "block.h"
#include "gameobject.h"
#include "entity.h"

#ifndef gamehelper_hpp
#define gamehelper_hpp
//work on ptrs

//returns center of object
namespace objutil {
#define toblock(a)(a.type==gameobject::block?(blockname::block*)(&a):nullptr)

#define toentity(a)((a.type==gameobject::entity)?(entity::entity*)(&a):nullptr)
	inline block& toblk(gameobject::obj* object) {

		return *((block*)(object));
	}
	inline block& toblk(gameobject::obj& object) {

		return *((block*)(&object));
	}inline entity::entity& toent(gameobject::obj* object) {

		return *((entity::entity*)(object));
	}
	inline entity::entity& toent(gameobject::obj& object) {

		return *((entity::entity*)(&object));
	}



	inline entity::entity* toent(gameobject::objref object)
	{

		return ((entity::entity*)(object.toobj()));
	}
	inline block* toblk(gameobject::objref object)
	{

		return ((block*)(object.toobj()));
	}

	inline Vector3 getcenter(gameobject::obj& object) {

	if (object.type==gameobject::block)
	{
		toblk(object).pos+ unitv / 2;
	}
	if (object.type==gameobject::entity)
	{
		((entity::entity*)(&object))->pos ;
	}
}
//returns id ob object only works for block


}
#endif // !gamehelper_Hpp

