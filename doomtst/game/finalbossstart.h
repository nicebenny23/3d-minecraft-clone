#include "../entities/antifreezefinalboss.h"
#ifndef finalbossstart_HPP
#define finalbossstart_HPP

void spawnfinalboss() {
	createfinalboss(Vector3(0, 10000, 0));
	player::goblin->transform.position = Vector3(0, 10010, 0);




}

void bringplayerbacktoloc() {

	player::goblin->transform.position = Vector3(0, 10000, 0);

}

#endif
