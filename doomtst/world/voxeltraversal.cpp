#include "voxeltraversal.h"
using namespace blockname;
using	 namespace voxtra;
using namespace grid;
//by implementing inner search loop acuracy scales quadraticlly
//aproxomate voxel traversel algorthm,the accuracy scales linearly with time complexity,
bool isvoxelvalid(block* blk, gridtrav trav) {
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
voxtra::RayCollisionWithGrid  voxtra::travvox(ray nray, float acc, gridtrav trav )
{
	float maxdist = nray.length();
	v3::Vector3 offdist = (nray.end - nray.start) / acc;
	v3::Vector3 pos = nray.start;
	Coord prevpos =grid::tovoxelspace( pos);
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
					if (isvoxelvalid(blk,trav))
					{
						aabb::aabbraycolinfo rayinfo = col->distanceonray(nray);
						if (rayinfo.collided)
						{


							return voxtra::RayCollisionWithGrid(rayinfo.dist, &blk->getcomponent<aabb::Collider>(), rayinfo.intersectionpoint);
						}
					}
				}
					
				
			}
		}
		prevpos = getvoxellocation(pos);
		pos += offdist;

	}
	return voxtra::RayCollisionWithGrid();
}

bool voxtra::raycolllideswithgrid(ray nray, float acc, gridtrav trav)
{
	float maxdist = nray.length();
	v3::Vector3 offdist = (nray.end - nray.start) / acc;
	v3::Vector3 pos = nray.start;
	Coord prevpos =grid::tovoxelspace( pos);
	for (int i = 0; i < maxdist * acc; i++)
	{
		Coord curvox = grid::getvoxellocation(pos);
		if (curvox != prevpos)
		{

			block* blk = grid::getobjatgrid(curvox);
			if (blk != nullptr)
			{
				
				if (isvoxelvalid(blk, trav))
				{
				
						aabb::aabbraycolinfo rayinfo = blk->getcomponent<aabb::Collider>().distanceonray(nray);
						if (rayinfo.collided)
						{


							return true;
						}
					
				}
			}
		}
		prevpos = getvoxellocation(pos);
		pos += offdist;

	}
	return false;
}

bool voxtra::Boxcollwithgrid(geometry::Box bx, gridtrav trav )
{

array<block*>& blks= 	grid::voxelinrange(bx);

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

			
				if (isvoxelvalid(tocollide, trav))
				{


					bool collided = aabb::aabbboxintersect(bx, blockcol);


					if (collided)
					{
						blks.destroy();
						return true;
					}
				}


	
	}
	blks.destroy();
	return false;




}

//todo remove this will not be needed as we will do a better function
	block* voxtra::findprevblock(ray nray, float acc,gridtrav trav)
	{
		float maxdist = nray.length();
		v3::Vector3 offdist = (nray.end - nray.start) / acc;
		v3::Vector3 pos = nray.start;
		Coord prevpos = pos;
	
		for (int i = 0; i < maxdist * acc; i++)
		{
			Coord curvox = grid::getvoxellocation(pos);
			if (curvox != prevpos)
			{
				
				block* blk = grid::getobjatgrid(curvox);
				if (blk != nullptr)
				{
		
					if (isvoxelvalid(blk, trav))
					{
						aabb::aabbraycolinfo rayinfo = blk->getcomponent<aabb::Collider>().distanceonray(nray);
						if (rayinfo.collided)
						{


							return grid::getobjatgrid(prevpos);
						}
					}
				}

			}

			prevpos = getvoxellocation(pos);
			pos += offdist;

		}
		return nullptr;
	}
