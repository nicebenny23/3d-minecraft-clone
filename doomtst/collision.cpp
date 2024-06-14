#include "collision.h"

void collision::update()
{
	for (int i = 0; i < colrectlist.length; i++)
	{
		if (colrectlist[i] != nullptr) {
			collideobjwithgrid(*colrectlist[i]);


		}
	}
}

void collision::collideobjwithgrid(colrect& entity)
{
	v3::Vector3 lowpos= entity.center - entity.scale-unitv;
	
	iv3::Ivector3 lowest = iv3::Ivector3(iv3::floorabs(lowpos.x), iv3::floorabs(lowpos.y), iv3::floorabs(lowpos.z));
	v3::Vector3 highpos = entity.center + entity.scale+unitv;

	iv3::Ivector3 highest= iv3::Ivector3(iv3::ceilabs(highpos.x), iv3::ceilabs(highpos.y), iv3::ceilabs(highpos.z));
	for (int  i = 0; i < 3; i++)
	{
		Vector3 minforce = zerov;
	
		for (int x = lowest.x; x < highest.x; x++)
		{
			for (int y = lowest.y; y < highest.y; y++)
			{
				for (int z = lowest.z; z < highest.z; z++)
				{
					block::rblock* tocollide = grid::getobjatgrid(x, y, z);
					if (tocollide != nullptr)
					{
						aabb::colrect blockcol = tocollide->collider;

						Vector3 force = aabb::collideaabb(entity, blockcol);
						if (force != zerov)
						{
							

								if (magnitude(force) > magnitude(minforce) || magnitude(minforce) == 0)
								{

									minforce = force;
								}
							

						}



					}
				}
			}
		}
		entity.center += minforce;
	}
}
