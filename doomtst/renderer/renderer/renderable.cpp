#include "renderable.h"
#include "../../game/GameContext.h"

renderer::renderable::renderable():object(CtxName::ctx.Ecs->spawn_empty()) {
	object.add_component<is_enabled>();
	object.add_component<order_key>(0);
	object.add_component<renderable_marker>();
	object.add_component<renderable_overides>();
}