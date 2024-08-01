#include "../entities/antifreezefinalboss.h"
#include "../renderer/blockrender.h"
#ifndef finalbossstart_HPP
#define finalbossstart_HPP

void spawnfinalboss() {
	createfinalboss(Vector3(0, 10000, 0));
	player::goblin->transform.position = Vector3(0, 10010, 0);
	blockrender::enablelighting = false;



}

void bringplayerbacktoloc() {

	//nostalic as this is were the player spawned
	player::goblin->transform.position = Vector3(0, 300, 0);

	blockrender::enablelighting = true;
}

#endif
