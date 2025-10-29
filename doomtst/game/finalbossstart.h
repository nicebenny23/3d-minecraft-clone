#pragma once
#include "../entities/antifreezeboss.h"
#include "../renderer/blockrender.h"
#include "../player/player.h"
 
void spawnfinalboss() {

}

void bringplayerbacktoloc() {

	//nostalic as this is were the player spawned
//player::goblin->transform.position = Vector3(0, 300, 0);

//	blockrender::enablelighting = true;
}
void testgameifspawn() {

	//if (entityname::shouldspawnfinalboss)
	{
		spawnfinalboss();
		//entityname::shouldspawnfinalboss = false;
	}

}

