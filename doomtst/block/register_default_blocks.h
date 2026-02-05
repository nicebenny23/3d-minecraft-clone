#include "air.h"
#include "stone.h"
#include "moss.h"
#include "torch.h"
#include "plank.h"

#pragma once
namespace blocks{
	void register_blocks(ecs::Ecs& world){
		BlockRegistry& registry=world.ensure_resource<BlockRegistry>();
		registry.register_block<PlankBlock>(minecrafttreestone);
		registry.register_block<MossBlock>(minecraftmoss);
		registry.register_block<TorchBlock>(minecrafttorch);
		registry.register_block<StoneBlock>(minecraftstone);
		registry.register_block<AirBlock>(minecraftair);


	}



}