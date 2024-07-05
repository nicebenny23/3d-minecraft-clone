#include "collision.h"
#include "objecthelper.h"
using namespace objutil;
void collision::update()
{
	for (int i = 0; i < colrectlist.length; i++)
	{
		if (colrectlist[i] != nullptr) {
			collideobjwithgrid(*colrectlist[i]);


		}
	}
}


collision::raycolwithgrid collision::collideraywithgrid(ray nray) {
	
	raycolwithgrid closest = raycolwithgrid();
	closest.dist = INFINITY;
	for (int i = 0; i < (loadamt * 2 + 1) * (loadamt * 2 + 1)*(2*loadamt+1); i++)
	{
		for (int j = 0;j < 16 * 16 * 16;j++) {
			block* blk = &grid::chunklist[i]->blockstruct[j];

			//for now gyrantee tha it has no aabb
			if (blk->id != minecraftair) {
				if (distance(nray.start, blk->pos  + unitv / 2) <nray.length())
				{


					aabbraycolinfo blkinter = blk->getcomponent<colrect>().distanceonray(nray);
					if (blkinter.collided&&blkinter.dist < closest.dist)
					{
						closest.colpoint= blkinter.intersectionpoint;
						closest.box =&( blk->getcomponent<colrect>());
						closest.dist = blkinter.dist;
					}
				}
			}

		}
	}
		return closest;
	
	
	
}
Vector3 getplaceoffset(Vector3 inter, Vector3 center,Vector3 colrectscale) {

	Vector3 pos = zerov;
	pos.x = v3::floorabs((inter.x - center.x)/colrectscale.x);
	pos.y = v3::floorabs((inter.y - center.y) / colrectscale.y);
	pos.z = v3::floorabs((inter.z - center.z) / colrectscale.z);
	return pos;
}
void collision::collidecamray() {
	ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
	raycolwithgrid closest = collideraywithgrid(cameraray);

	if ( closest.box != nullptr)
	{

	
		if (interactminrange < closest.dist && closest.dist < interactmaxrange)
		{


			if (userinput::mouseleft.pressed)
			{
				setair((block*)(closest.box->owner));
			}


			else if (userinput::mouseright.pressed)
			{

				Coord placmentpoint = getplaceoffset(closest.colpoint, closest.box->center, closest.box->scale);
				block* plamentblock = grid::getobjatgrid(((block*)(closest.box->owner))->pos + placmentpoint);
				if (plamentblock != nullptr)            
				{                                         
					plamentblock->id = minecraftstone;
					giveblocktraits(plamentblock);
				}

			}
		}

	}
}


void collision::collideobjwithgrid(colrect& entity)
{
	v3::Vector3 lowpos= entity.center - entity.scale-unitv;
	
	v3::Coord lowest = v3::Coord(v3::floorabs(lowpos.x), v3::floorabs(lowpos.y), v3::floorabs(lowpos.z));
	v3::Vector3 highpos = entity.center + entity.scale+unitv;

	v3::Coord highest= v3::Coord(v3::ceilabs(highpos.x), v3::ceilabs(highpos.y), v3::ceilabs(highpos.z));
	for (int  i = 0; i < 3; i++)
	{
		Vector3 minforce = zerov;
	
		for (int x = lowest.x; x < highest.x; x++)
		{
			for (int y = lowest.y; y < highest.y; y++)
			{
				for (int z = lowest.z; z < highest.z; z++)
				{
					//getting solid at grid is optimization

					blockname::block* tocollide = grid::getobjatgrid(x, y, z,false);
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
