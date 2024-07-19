#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
#include "../rigidbody.h"
using namespace objutil;
void collision::update()
{
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr) {
			handleCollisionWithGrid(*Colliderlist[i]);


		}
	}
	handleduelentitycollisions();
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr) {

			Colliderlist[i]->prevpos = Colliderlist[i]->center;
		}
	}
}

voxtra::RayCollisionWithGrid collision::raycastCollisionWithGrid(ray nray) {

	voxtra::RayCollisionWithGrid closest = voxtra::RayCollisionWithGrid();
	closest.dist = INFINITY;
	for (int i = 0; i < totalgridsize; i++)
	{
		for (int j = 0; j < chunksize; j++) {
			block* blk = &grid::chunklist[i]->blockbuf[j];

			//for now gyrantee tha it has no aabb
			if (blk->solid) {
				if (distance(nray.start, blk->center()) < nray.length())
				{


					aabbraycolinfo blkinter = blk->getcomponent<Collider>().distanceonray(nray);
					if (blkinter.collided && blkinter.dist < closest.dist)
					{
						closest.colpoint = blkinter.intersectionpoint;
						closest.box = &(blk->getcomponent<Collider>());
						closest.dist = blkinter.dist;
					}
				}
			}

		}
	}
	return closest;



}
void componentcollisionsend(gameobject::obj* reciever, gameobject::obj* collided) {

	for (int i = 0; i < reciever->complist.length; i++)
	{
		reciever->complist[i]->oncollision(collided);
	}
}
void collision::handleduelentitycollisions()
{
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] == nullptr) {
			continue;
		}
		for (int j = i; j < Colliderlist.length; j++)
		{
			if (i == j) {
				continue;
			}

			if (Colliderlist[i] == nullptr || Colliderlist[j] == nullptr) {

				continue;
			}
			v3::Vector3 force = aabb::collideaabb(*Colliderlist[i], *Colliderlist[j]);
			if (force != zerov)
			{
				Vector3 actualforce = force / 2;

				Colliderlist[i]->center += actualforce;
				toent(Colliderlist[i]->owner).pos += actualforce;
				componentcollisionsend(Colliderlist[i]->owner, Colliderlist[j]->owner);
				Colliderlist[j]->center -= actualforce;
				toent(Colliderlist[j]->owner).pos -= actualforce;
				componentcollisionsend(Colliderlist[j]->owner, Colliderlist[i]->owner);


			}

		}
	}
}
Vector3 getplaceoffset(Vector3 inter, Vector3 center, Vector3 Colliderscale) {

	Vector3 pos = zerov;
	pos.x = floorabs((inter.x - center.x) / Colliderscale.x);
	pos.y = floorabs((inter.y - center.y) / Colliderscale.y);
	pos.z = floorabs((inter.z - center.z) / Colliderscale.z);
	return pos;
}
bool collision::aabbCollidesWithEntity(Collider* blk) {
	for (int i = 0; i < Colliderlist.length; i++)
	{
		if (Colliderlist[i] != nullptr)
		{


			if (aabbsintersect(*(Colliderlist[i]), *blk))
			{
				return true;
			}
		}
	}
	return false;
}

//todo implement movment per axis
void collision::handleCollisionWithGrid(Collider& entity)
{
	entity.center = toent(entity.owner).pos;
	v3::Vector3 lowpos = entity.center - entity.scale - unitv;

	v3::Coord lowest = v3::Coord(floorabs(lowpos.x), floorabs(lowpos.y), floorabs(lowpos.z));
	v3::Vector3 highpos = entity.center + entity.scale + unitv;

	v3::Coord highest = v3::Coord(ceilabs(highpos.x), ceilabs(highpos.y), ceilabs(highpos.z));
	std::swap(entity.center, entity.prevpos);
	for (int i = 0; i < 3; i++)
	{
		switch (i)
		{

		case 0:
			entity.center.y = entity.prevpos.y;
		case 1:
			entity.center.z = entity.prevpos.z;
		case 2:
			entity.center.x = entity.prevpos.x;
		}
		Vector3 minforce = zerov;

		for (int x = lowest.x; x < highest.x; x++)
		{
			for (int y = lowest.y; y < highest.y; y++)
			{
				for (int z = lowest.z; z < highest.z; z++)
				{
					//getting solid at grid is optimization

					blockname::block* tocollide = grid::getobjatgrid(x, y, z, false);
					if (tocollide != nullptr)
					{
						if (tocollide->hascomponent<aabb::Collider>())
						{


							aabb::Collider& blockcol = tocollide->getcomponent<aabb::Collider>();



							Vector3 force = aabb::collideaabb(entity, blockcol);


							if (force != zerov)
							{
								for (int i = 0; i < entity.owner->complist.length; i++)
								{
									entity.owner->complist[i]->oncollision(tocollide);
								}for (int i = 0; i < tocollide->complist.length; i++)
								{
									tocollide->complist[i]->oncollision(entity.owner);
								}


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





		entity.prevpos += minforce;
		entity.center += minforce;
		toent(entity.owner).pos += minforce;
	}


}


