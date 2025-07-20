#include "entitystate.h"
#include "rigidbody.h"
#include "objecthelper.h"
#include "../items/loottable.h"
void estate::remove()
{
	if (owner.hascomponent<loottable>())
	{
		owner.getcomponent<loottable>().should_drop = true;
	}

	if (!owner.hascomponent<gameobject::StaticComponent>())
	{

		owner.destroy();
	}
}
