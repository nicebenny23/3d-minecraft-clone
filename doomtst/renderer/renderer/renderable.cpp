#include "renderable.h"
#include "../../game/GameContext.h"

renderer::renderable::renderable(renderable_id id) :id(id), object(CtxName::ctx.Ecs->spawn_empty()) {
	object.add_component<is_enabled>();
	object.add_component<order_key>(0);
}