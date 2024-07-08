#include "voxeltraversal.h"
using namespace blockname;
//by implementing inner search loop acuracy scales quadraticlly
//aproxomate voxel traversel algorthm,the accuracy scales linearly with time complexity,
raycolwithgrid  travvox(ray nray,float acc)
{
	float maxdist = nray.length();
	v3::Vector3 offdist = (nray.end - nray.start) / acc;
	v3::Vector3 pos = nray.start;
	Coord prevpos = pos;
	for (int i = 0; i < maxdist*acc; i++)
	{
		Coord curvox= getcurrvoxel(pos);
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


						return raycolwithgrid(rayinfo.dist, &blk->getcomponent<aabb::colrect>(), rayinfo.intersectionpoint);
					}
				}
			}
		}
		prevpos = getcurrvoxel(pos);
			pos += offdist;
			
	}
	return raycolwithgrid();
}
block* findprevblock(ray nray, float acc)
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
