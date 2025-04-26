#include "rigidbody.h"
#include "entitystate.h"
#ifndef  entityutil_HPP
#define entityutil_HPP


inline void kb(Vector3 point, float force,Ent::entity* ent) {
	if (ent->hascomponent<estate>())
	{


		if (ent->getcomponent<estate>().timetilldmg <= 0)
		{


			if (ent->hascomponent<rigidbody>())
			{
				Vector3 forceval = point -objutil::toent(ent).transform.position;
				forceval = normal(forceval);
				forceval *= force;

				ent->getcomponent<rigidbody>().velocity -= forceval;
			}
		}
	}
}
#endif // ! entityutil_HPP
