#include "ecs.h"
#include "game_object.h"
namespace ecs {

	obj Ecs::spawn_empty()
	{
		//adds to both
		entity new_entity = entities.allocate_space_id();
		archetypes.add_to_empty(new_entity);
		return obj(new_entity, this);
	}
}