#include "block.h"
#include "gameobject.h"
#include "entity.h"

#ifndef gamehelper_hpp
#define gamehelper_hpp
//work on ptrs
#define toblock(a)(a.type==gameobject::block?(blockname::block*)(&a):nullptr)

#define toentity(a)((a.type==gameobject::entity)?(entity::entity*)(&a):nullptr)
//returns center of object

inline Vector3 center(gameobject::obj object) {

	if (object.type==gameobject::block)
	{
		Vector3(toblock(object)->pos) + unitv / 2;
	}
	if (object.type==gameobject::entity)
	{
		return toentity(object)->pos;
	}
}
//returns id ob object only works for block
inline int& id(gameobject::obj object) {


	if (object.type==gameobject::block)
	{
		return (toblock(object)->id);
	}
	if (object.type==gameobject::entity)
	{
		_STATIC_ASSERT("attempted to acess id of entity");
	}
}
//returns id ob object only works for block
inline v3::Vector3& pos(gameobject::obj object) {


	if (object.type == gameobject::block)
	{
		_STATIC_ASSERT("attempted to acess pos of block,doesent exist");
	}
	if (object.type == gameobject::entity)
	{
		return (toentity(object)->pos);
	}
}
#endif // !gamehelper_Hpp

