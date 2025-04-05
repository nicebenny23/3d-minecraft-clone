#include "voxeltraversal.h"
using namespace blockname;
using	 namespace voxtra;
using namespace grid;
//by implementing inner search loop acuracy scales quadraticlly
//aproxomate voxel traversel algorthm,the accuracy scales linearly with time complexity,
bool counttablevoxel(block* blk, gridtrav trav) {
	if ((trav == countsolid || trav == countnormal) && !blk->attributes.solid)
	{
		return false;

	}


	
	if (blk->getcomponent<aabb::Collider>().effector )
	{
		return trav != countnormal;

	}
	return true;
}

bool voxtra::Boxcollwithgrid(geometry::Box bx, gridtrav trav )
{

array<block*>& blks= grid::voxelinrange(bx);

	for (int ind=0; ind < blks.length; ind++)
	{
		

		blockname::block* tocollide = blks.at(ind);
				if (tocollide == nullptr)
				{
					continue;
				}
				if (!tocollide->hascomponent<aabb::Collider>())
				{
					continue;
				}
				aabb::Collider& blockcol = tocollide->getcomponent<aabb::Collider>();

			
				if (counttablevoxel(tocollide, trav))
				{


				


					if (aabb::aabbboxintersect(bx, blockcol))
					{
						blks.destroy();
						return true;
					}
				}


	
	}
	blks.destroy();
	return false;




}

voxtra::RayWorldIntersection  voxtra::travvox(ray nray, float acc, gridtrav trav)
{
	float maxdist = nray.length();
	v3::Vector3 offdist = (nray.end - nray.start) / acc;
	v3::Vector3 pos = nray.start;
	Coord prevpos = grid::scaletoblocksize(pos);
	for (int i = 0; i < maxdist * acc; i++)
	{
		Coord curvox = grid::getvoxellocation(pos);
		if (curvox != prevpos)
		{

			block* blk = grid::getobjatgrid(curvox);
			if (blk != nullptr)
			{

				aabb::Collider* col = blk->getcomponentptr<aabb::Collider>();
				if (col != nullptr)
				{
					if (counttablevoxel(blk, trav))
					{
						geointersect::boxraycollision rayinfo = geointersect::intersection(col->globalbox(), nray);
						if (rayinfo.collided)
						{


							return voxtra::RayWorldIntersection(rayinfo.dist, &blk->getcomponent<aabb::Collider>(), rayinfo.intersectionpoint, nray);
						}
					}
				}


			}
		}
		prevpos = getvoxellocation(pos);
		pos += offdist;

	}
	return voxtra::RayWorldIntersection();
}

//todo remove this will not be needed as we will do a better function
//essentially goes to the next block and then goes backwords
	block* voxtra::findprevblock(ray nray, float acc,gridtrav trav)
	{
		RayWorldIntersection inter=  travvox(nray, acc, trav);
		if (!inter.intersected())
		{
			return nullptr;
		}
		Vector3 point = inter.colpoint;
		point -= nray.dir() * .01f;
	return	(grid::getobjatgrid(grid::getvoxellocation( point)));
	}
