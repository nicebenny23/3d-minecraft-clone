#include "rigidbody.h"
#include "entitystate.h"
#pragma once 
inline void kb(Vector3 point, float force,gameobject::obj ent) {
	if (ent.hascomponent<estate>()&&ent.hascomponent<gameobject::transform_comp>())
	{


		if (ent.getcomponent<estate>().timetilldmg <= 0)
		{


			if (ent.hascomponent<rigidbody>())
			{
				Vector3 forceval = point -ent.transform().position;
				forceval = normal(forceval);
				forceval *= force;

				ent.getcomponent<rigidbody>().velocity -= forceval;
			}
		}
	}
}
 // ! entityutil_HPP
