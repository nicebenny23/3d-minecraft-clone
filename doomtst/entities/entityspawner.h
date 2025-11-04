#include "../player/player.h"
#include "../util/random.h"
#include "slime.h"
#include "../world/grid.h"
#include"../world/voxeltraversal.h"
#include "../renderer/blockrender.h"
#include "../game/System.h"
#include "../player/player.h"
#pragma once 
#define enemytag 1
inline bool should_despawn(v3::Vec3 pos, v3::Vec3 player) {

	return 24<v3::dist(pos, player);

}
inline bool spawnable_dist(v3::Vec3 pos, v3::Vec3 player) {

	return !should_despawn(pos,player)&&8<v3::dist(pos, player);

}

struct spawn_mobs :System {
	bool ensure_light_level(v3::Vec3 pos) {
		blockname::block* blk = CtxName::ctx.Grid->getBlock(CtxName::ctx.Grid->getVoxel(pos));
		if (!blk)
		{
			return false;
		}
		float average = 0;
		for (int i = 0; i < 6; i++)
		{
			average += blk[i].lightval / static_cast<float>(6);
		
		}
		return 	(average < 8);
		}

	stn::Option<geometry::Box> find_spawn_location(v3::Vec3 Scl) {
		size_t max_checks = 40;
		for (size_t chks = 0; chks < max_checks; chks++)
		{


			stn::Option<Box> spawn_loc = voxtra::findground(unitv);
			if (!spawn_loc)
			{
				return stn::None;
			}
			v3::Vec3 pos = spawn_loc.unwrap().center;
			if (!ensure_light_level(pos))
			{
				continue;
			}
			if (!spawnable_dist(pos,player::goblin.transform().position))
			{
				continue;
			}
			return spawn_loc;
		}
		return stn::None;
	}
	void run(gameobject::Ecs* ecs) {
		return;
		if (!blockrender::enablelighting)
		{
			return;
		}
		float spawnthreshold = .01f;
		float randomnum = random();
		size_t total_alive = 0;

		query::View<slimemove> slimes(ecs);
			for (auto [slime] : slimes) {

				if (should_despawn(slime->owner.transform().position,player::goblin.transform().position))
				{
					slime->owner.destroy();
				}
				total_alive++;




			}
			const size_t max_alive=1;
			if (max_alive<=total_alive)
			{
				return;
			}
			query::View<player::player_tag> players(ecs);
			for (auto [player] : players) {
			{


				if (randomnum < spawnthreshold)
				{

					stn::Option<Box> spawn_loc = find_spawn_location(unitv);
					if (!spawn_loc)
					{
						continue;
					}
 					createslime(spawn_loc.unwrap().center, false);
					
				



				}
			}
		}
	}

};

 // !entityspawner_HPP
