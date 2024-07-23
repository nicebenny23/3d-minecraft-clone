#include "collision.h"
#include "objecthelper.h"
#include "../world/managegrid.h"
#include "../game/rigidbody.h"
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

void collision::sendplayercameraray()
{

	ray cameraray = ray(Vector3(camera::campos), Vector3(camera::campos) + camera::direction() * 7);
	voxtra::RayCollisionWithGrid 	closest = collision::raycastall(cameraray);
	if (closest.box!=nullptr)
	{
		for (int i = 0; i < closest.box->owner->complist.length; i++) {
			closest.box->owner->complist[i]->onplayerclick();

		}
	}
}

voxtra::RayCollisionWithGrid collision::raycastentity(ray nray)
{
	voxtra::RayCollisionWithGrid closest = voxtra::RayCollisionWithGrid();
	closest.dist = INFINITY;

	for (int i = 0; i < entityname::objectfromguid.length; i++)
	{
		if (entityname::objectfromguid[i] != nullptr) {
			entityname::entity *entatpos = entityname::objectfromguid[i];
				

			if (entatpos->hascomponent<Collider>())
			{
				Collider* coll = &entatpos->getcomponent<Collider>();
				if (distance(coll->center,nray.start)<nray.length())
				{
				
					aabbraycolinfo blkinter =coll->distanceonray(nray);
					if (blkinter.collided && blkinter.dist < closest.dist)
					{
						closest.colpoint = blkinter.intersectionpoint;
						closest.box =coll;
						closest.dist = blkinter.dist;
					}
				}
			}

		}
	}
	return closest;
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

		if (Colliderlist[i] != nullptr) {
			Colliderlist[i]->center = objutil::toent(Colliderlist[i]->owner).transform.position;
		}
	}

	for (int i = 0; i < Colliderlist.length; i++)
	{
		
		if (Colliderlist[i] == nullptr) {
			continue;
		}
		for (int j = 0; j < Colliderlist.length; j++)
		{
			if (i == j) {
				continue;
			}

			if (Colliderlist[i] == nullptr || Colliderlist[j] == nullptr) {

				continue;
			}

			v3::Vector3 force = aabb::collideaabb(*Colliderlist[i], *Colliderlist[j]);
			if (v3::magnitude( force )>0.00001f)
			{
				Vector3 actualforce = force / 2.0f;
				componentcollisionsend(Colliderlist[i]->owner, Colliderlist[j]->owner);
				componentcollisionsend(Colliderlist[j]->owner, Colliderlist[i]->owner);

				if (Colliderlist[i]->effector || Colliderlist[j]->effector) {

					continue;
				}
				Colliderlist[i]->center += actualforce;
				toent(Colliderlist[i]->owner).transform.position += actualforce;
				
				Colliderlist[j]->center -= actualforce;
				toent(Colliderlist[j]->owner).transform.position -= actualforce;


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

voxtra::RayCollisionWithGrid collision::raycastall(ray nray)
{
	voxtra::RayCollisionWithGrid gridcol = voxtra::travvox(nray,100);
	voxtra::RayCollisionWithGrid entcol = raycastentity(nray);
	if (gridcol.dist<entcol.dist)
	{
		return gridcol;
	}
	return entcol;
}

//todo implement movment per axis
void collision::handleCollisionWithGrid(Collider& entity)
{
	entity.center = toent(entity.owner).transform.position;
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


							if (force == zerov)
							{
								continue;
							}


								for (int i = 0; i < entity.owner->complist.length; i++)
								{
									entity.owner->complist[i]->oncollision(tocollide);
								}for (int i = 0; i < tocollide->complist.length; i++)
								{
									tocollide->complist[i]->oncollision(entity.owner);
								}

								componentcollisionsend(entity.owner, tocollide);
								componentcollisionsend(tocollide, entity.owner);

								if (!blockcol.effector&&!entity.effector)
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





		entity.prevpos += minforce;
		entity.center += minforce;
		toent(entity.owner).transform.position += minforce;
	}


}


