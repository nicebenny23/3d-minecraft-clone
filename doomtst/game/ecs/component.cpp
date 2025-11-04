#include "ecs.h"
#include "game_object.h"
namespace ecs{

	obj component::owner()
	{
		return obj(ent, ecs);
	}
}