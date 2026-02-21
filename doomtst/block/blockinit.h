#include "block.h"	
#include "block_registry.h"
#include "../game/aabb.h"
#include "../util/fileloader.h"

#pragma once  
namespace blocks {


	struct GenerateBlock{
		blocks::block_id id;
		v3::Coord loc;
		math::Direction2d face;
		math::Direction3d direction;
		Chunk::chunkmesh& mesh;
		stn::Option<stn::file_handle&> handle;
		
		//it cannot be a recipe due to its optimizations mechanics
		ecs::obj spawn(ecs::Ecs& world) {
			BlockRegistry& registry =world.insert_resource<BlockRegistry>();
			BlockTraits traits=registry.traits_for(id);
			block& blk = world.spawn_with_component<block>(traits.mesh, mesh, loc, id, direction, face,traits.solid);
			ecs::obj object = blk.owner();
			if (traits.emmited_light!=0) {
				object.add_component<block_emmision>(traits.emmited_light);
			}
			if (traits.solid) {
				object.add_component<aabb::Collider>(geo::Box(v3::Point3(0,0,0), traits.mesh.size),!traits.solid);
			}
			if (traits.mesh.transparent) {
				blk.mesh.transparent = true;
			}
			if (handle) {

				registry.block_for(id)->read_from_bytes(object, handle.unwrap());
			}
			else {
				registry.block_for(id)->apply(object);
			}
			return blk.owner();
		}
	};

}