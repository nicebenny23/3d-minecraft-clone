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
	inline entity::entity* entity(gameobject::objref object)
	{

		return ((entity::entity*)(object.toobj()));
	}
	inline Vector3 getcenter(gameobject::obj object) {

	if (object.type==gameobject::block)
	{
		vec(toblock(object)->pos) + unitv / 2;
	}
	if (object.type==gameobject::entity)
	{
		return toentity(object)->pos;
	}
}
//returns id ob object only works for block
inline byte& getid(gameobject::obj& object) {


	if (object.type==gameobject::block)
	{
		return ((block*)(&object))->id;
	}
	if (object.type==gameobject::entity)
	{
		_STATIC_ASSERT("attempted to acess id of entity");
	}
}
inline byte& getid(gameobject::obj* object) {


	if (object->type == gameobject::block)
	{
		return ((block*)(object))->id;
	}
	if (object->type == gameobject::entity)
	{
		_STATIC_ASSERT("attempted to acess id of entity");
	}
}
//returns id ob object only works for block
inline v3::Vector3& getpos(gameobject::obj object) {


	if (object.type == gameobject::block)
	{
		_STATIC_ASSERT("attempted to acess pos of block,doesent exist");
	}
	if (object.type == gameobject::entity)
	{
		return ((entity::entity*)(&object))->pos;
	}
}
inline v3::Vector3& getpos(gameobject::obj* object) {


	if (object->type == gameobject::block)
	{
		_STATIC_ASSERT("attempted to acess pos of block,doesent exist");
	}
	if (object->type == gameobject::entity)
	{
		return ((entity::entity*)(object))->pos;
	}
}
inline v3::Vector3& getpos(gameobject::objref object) {
	if (object.toobj()==nullptr)
	{
		_STATIC_ASSERT("cannot get pos of deleted entity");
	}
	if (object.toobj()->type == gameobject::entity)
	{
		return ((entity::entity*)(object.toobj()))->pos;
	}
	if (object.toobj()->type == gameobject::block)
	{
		_STATIC_ASSERT("attempted to acess pos of block,doesent exist");
	}
	
}
}
#endif // !gamehelper_Hpp

