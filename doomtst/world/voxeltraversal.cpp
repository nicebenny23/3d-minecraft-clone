#include "voxeltraversal.h"
using namespace blockname;
//by implementing inner search loop acuracy scales quadraticlly
//aproxomate voxel traversel algorthm,the accuracy scales linearly with time complexity,

voxtra::raycolwithgrid  voxtra::travvox(ray nray, float acc)
{
	float maxdist = nray.length();
	v3::Vector3 offdist = (nray.end - nray.start) / acc;
	v3::Vector3 pos = nray.start;
	Coord prevpos = pos;
	for (int i = 0; i < maxdist * acc; i++)
	{
		Coord curvox = voxtra::getcurrvoxel(pos);
		if (curvox != prevpos)
		{

			block* blk = grid::getobjatgrid(curvox);
			if (blk != nullptr)
			{
				if (blk->solid)
				{
					aabb::aabbraycolinfo rayinfo = blk->getcomponent<aabb::colrect>().distanceonray(nray);
					if (rayinfo.collided)
					{


						return voxtra::raycolwithgrid(rayinfo.dist, &blk->getcomponent<aabb::colrect>(), rayinfo.intersectionpoint);
					}
				}
			}
		}
		prevpos = getcurrvoxel(pos);
		pos += offdist;

	}
	return voxtra::raycolwithgrid();
}

bool voxtra::Boxcollwithgrid (geometry::Box bx, float acc)
{

	v3::Vector3 lowpos = bx.center - bx.scale - unitv;

	v3::Coord lowest = v3::Coord(floorabs(lowpos.x), floorabs(lowpos.y), floorabs(lowpos.z));
	v3::Vector3 highpos = bx.center + bx.scale + unitv;

	v3::Coord highest = v3::Coord(ceilabs(highpos.x), ceilabs(highpos.y), ceilabs(highpos.z));
	
	for (int x = lowest.x; x < highest.x; x++)
	{
		for (int y = lowest.y; y < highest.y; y++)
		{
			for (int z = lowest.z; z < highest.z; z++)
			{
				
				blockname::block* tocollide = grid::getobjatgrid(x, y, z, false);
				if (tocollide == nullptr)
				{
					continue;
				}
				if (!tocollide->hascomponent<aabb::colrect>())
				{
				    continue;
				}
						aabb::colrect& blockcol = tocollide->getcomponent<aabb::colrect>();



						bool collided = aabb::aabbboxintersect(bx, blockcol);


						if (collided )
						{
							return true;
						}
						

				}
		}


	}
	return false;
}








	block* voxtra::findprevblock(ray nray, float acc)
	{
		float maxdist = nray.length();
		v3::Vector3 offdist = (nray.end - nray.start) / acc;
		v3::Vector3 pos = nray.start;
		Coord prevpos = pos;
		block* prevblock;
		for (int i = 0; i < maxdist * acc; i++)
		{
			Coord curvox = getcurrvoxel(pos);
			if (curvox != prevpos)
			{
				prevblock = grid::getobjatgrid(prevpos);
				block* blk = grid::getobjatgrid(curvox);
				if (blk != nullptr)
				{
					if (blk->solid)
					{
						aabb::aabbraycolinfo rayinfo = blk->getcomponent<aabb::colrect>().distanceonray(nray);
						if (rayinfo.collided)
						{


							return prevblock;
						}
					}
				}

			}

			prevpos = getcurrvoxel(pos);
			pos += offdist;

		}
		return nullptr;
	}
