#include "ecs.h"
#include "game_object.h"
namespace ecs{

	obj component::owner() const
	{
		return obj(ent, *ecs);
	}
}