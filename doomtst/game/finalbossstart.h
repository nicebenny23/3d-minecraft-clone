#pragma once
#include "../entities/antifreezeboss.h"
#include "../renderer/blockrender.h"
#include "../player/player.h"
 
void spawnfinalboss() {
	createfinalboss(Vector3(0, 10020, 0));
	player::goblin->transform.position = Vector3(0, 10010, 0);
	blockrender::enablelighting = false;



}

void bringplayerbacktoloc() {

	//nostalic as this is were the player spawned
	player::goblin->transform.position = Vector3(0, 300, 0);

	blockrender::enablelighting = true;
}
void testgameifspawn() {

	if (entityname::shouldspawnfinalboss)
	{
		spawnfinalboss();
		entityname::shouldspawnfinalboss = false;
	}

}

