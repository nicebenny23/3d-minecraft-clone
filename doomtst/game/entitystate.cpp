#include "entitystate.h"
#include "rigidbody.h"
#include "objecthelper.h"
#include "../items/loottable.h"
void estate::remove()
{
	if (owner().has_component<loottable>())
	{
		owner().get_component<loottable>().should_drop = true;
	}

	//if (!owner().has_component<ecs::StaticComponent>())
	{
		if (owner().has_component<ecs::transform_comp>())
		{
			int l = 1;
		}
		owner().destroy();
	}
}
