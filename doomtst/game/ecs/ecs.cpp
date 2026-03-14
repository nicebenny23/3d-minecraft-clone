#include "ecs.h"
#include "game_object.h"
namespace ecs {
	obj Ecs::object_from_entity(ecs::entity ent) {
		return obj(ent, *this);
	}

	obj Ecs::spawn_empty() {
		entity new_entity = entities.allocate_entity();
		archetypes.add_to_empty(new_entity.id());
		return object_from_entity(new_entity);
	}

}