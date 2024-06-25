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
Vector3 specfloor(Vector3 inter,Vector3 center) {
	
	v3::Vector3 pos = zerov;
	pos.x = iv3::floorabs((inter.x - center.x) * 2);
	pos.y = iv3::floorabs((inter.y - center.y) * 2);
	pos.z= iv3::floorabs((inter.z	- center.z) * 2);
	return pos;
}
void collision::collideraywithgrid() {
	ray cameraray = ray(v3::Vector3(camera::campos), v3::Vector3(camera::campos) + camera::direction());
	block* closest = nullptr;
	aabbraycollision closestblock = aabbraycollision();
	for (int i = 0; i < (loadamt * 2 + 1) * (loadamt * 2 + 1); i++)
	{
		for (int j = 0;j < 16 * 16 * 16;j++) {
			block* bl = &grid::chunklist[i]->blockstruct[j];
			if (bl->hascomponent<colrect>()) {

				aabbraycollision inter= bl->getcomponent<colrect>().distanceonray(cameraray);
				if (inter.dist <closestblock.dist &&bl->id!=minecraftair)
				{
				closestblock= inter;
				closest = bl;
					
				}
			}

		}
	}
	if (2< closestblock.dist) {
		if (closest != nullptr && userinput::mouseleft.pressed && closestblock.dist < 115)
		{
			closest->id = minecraftair;
			closest->removecomponent<colrect>();
		}
		if (closest != nullptr && userinput::mouseright.pressed && closestblock.dist < 511)
		{

			v3::Vector3 pointwhereblockplace = v3::Vector3(closest->pos) + specfloor(closestblock.intersectionpoint, closest->getcomponent<colrect>().center);
			block* toplace = grid::getobjatgrid2(pointwhereblockplace.x, pointwhereblockplace.y, pointwhereblockplace.z);
			if (toplace != nullptr)
			{
				toplace->id = minecraftgrass;
				toplace->texture = 1;
				toplace->createaabb();
			}

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
					blockname::block* tocollide = grid::getobjatgrid(x, y, z);
					if (tocollide != nullptr)
					{
						if (tocollide->hascomponent<aabb::colrect>())
						{


							aabb::colrect blockcol = tocollide->getcomponent<aabb::colrect>();
						
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
		}
		entity.center += minforce;
	}
}
