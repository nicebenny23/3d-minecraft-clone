#include "rigidbody.h"
#include "entitystate.h"
#pragma once 
inline void kb(Vec3 point, float force,gameobject::obj ent) {
	if (ent.hascomponent<estate>()&&ent.hascomponent<gameobject::transform_comp>())
	{


		if (ent.getcomponent<estate>().timetilldmg <= 0)
		{


			if (ent.hascomponent<rigidbody>())
			{
				Vec3 forceval = ent.transform().position- point;
				forceval = Vec3(forceval.x, .25, forceval.z);
				forceval = normal(forceval);
				
				forceval *= force;

				ent.getcomponent<rigidbody>().add_force( rigid_force(forceval*3,.333333333, "kb_force"));
			}
		}
	}
}
 // ! entityutil_HPP
