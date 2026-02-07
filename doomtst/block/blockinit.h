#include "block.h"	
#include "block_registry.h"
#include "../game/aabb.h"

#pragma once  
namespace blocks {


	struct GenerateBlock{
		blocks::block_id id;
		v3::Coord loc;
		math::Direction2d face;
		math::Direction3d direction;
		Chunk::chunkmesh& mesh;
		GenerateBlock(Chunk::chunkmesh& block_chunk_mesh,blocks::block_id blkid, v3::Coord location, math::Direction2d face_to_attach, math::Direction3d direction_to_attach)
			:mesh(block_chunk_mesh), id(blkid), loc(location), face(face_to_attach), direction(direction_to_attach) {

		}
		//it cannot be a recipe due to its optimizations mechanics
		ecs::obj spawn(ecs::Ecs& world) {
			BlockRegistry& registry =world.ensure_resource<BlockRegistry>();
			BlockTraits& traits=registry.traits_for(id);
			
			BlockType& block_type = registry.type_for(id); 
			block& blk = world.spawn_with_component<block>(traits.mesh, mesh, loc, id, direction, face,traits.solid, block_type.mining_traits());
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
			block_type.apply(object);
			return blk.owner();
		}
	};

}