#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
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

raycolwithgrid collision::collideraywithgrid(ray nray) {
	
	raycolwithgrid closest = raycolwithgrid();
	closest.dist = INFINITY;
	for (int i = 0; i < (loadamt * 2 + 1) * (loadamt * 2 + 1)*(2*loadamt+1); i++)
	{
		for (int j = 0;j < 16 * 16 * 16;j++) {
			block* blk = &grid::chunklist[i]->blockstruct[j];

			//for now gyrantee tha it has no aabb
			if (blk->solid) {
				if (distance(nray.start, blk->center()) <nray.length())
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
	pos.x = floorabs((inter.x - center.x)/colrectscale.x);
	pos.y = floorabs((inter.y - center.y) / colrectscale.y);
	pos.z = floorabs((inter.z - center.z) / colrectscale.z);
	return pos;
}
bool aabbcollideswithent(colrect* blk) {
	for (int i = 0; i < colrectlist.length; i++)
	{
		if (colrectlist[i] != nullptr)
		{


			if (aabbsintersect(*(colrectlist[i]), *blk))
			{
				return true;
			}
		}
	}
	return false;
}

//todo implement movment per axis
void collision::collideobjwithgrid(colrect& entity)
{
	v3::Vector3 lowpos= entity.center - entity.scale-unitv;
	
	v3::Coord lowest = v3::Coord(floorabs(lowpos.x), floorabs(lowpos.y), floorabs(lowpos.z));
	v3::Vector3 highpos = entity.center + entity.scale+unitv;

	v3::Coord highest= v3::Coord(ceilabs(highpos.x), ceilabs(highpos.y), ceilabs(highpos.z));
	std::swap(entity.center, entity.prevpos);
	for (int  i = 0; i < 3; i++)
	{
		switch (i)
		{

		case 0:
			entity.center.y= entity.prevpos.y;
		case 1:
			entity.center.z= entity.prevpos.z;
		case 2:
			entity.center.x= entity.prevpos.x;
		}
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
		entity.prevpos+= minforce;
		entity.prevpos = entity.center;
	}
}
void collision::collidecamray() {
	ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
	raycolwithgrid closest = travvox(cameraray, 1000);

	if (closest.box != nullptr)
	{



		if (userinput::mouseleft.pressed)
		{
			if (interactminrange < closest.dist && closest.dist < interactmaxrange)
			{


				gridutil::placeblockatloc(toblk(closest.box->owner).pos, minecraftair);
			}
		}


		else if (userinput::mouseright.pressed)
		{

			if (interactminrange < closest.dist && closest.dist < interactmaxrange)
			{
			
				block* plamentblock =findprevblock(cameraray,1000);
				if (plamentblock!= nullptr && !plamentblock->solid)
				{
					int previd = plamentblock->id;
					//i dont know why i create it and remove itit like this but it makes the core much simpler
					gridutil::placeblockatloc(plamentblock->pos, minecraftglass);
					if (aabbcollideswithent(&plamentblock->getcomponent<colrect>()))
					{

						gridutil::placeblockatloc(plamentblock->pos, previd);
					}
				}

			}


		}

	}
}