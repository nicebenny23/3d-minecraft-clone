#include "block.h"	
#include "block_registry.h"

#pragma once  
namespace blocks {


	struct GenerateBlock :ecs::Recipe {
		blocks::block_id id;
		v3::Coord loc;
		Dir::Dir2d face;
		Dir::Dir3d direction;
		Chunk::chunkmesh& mesh;
		GenerateBlock(Chunk::chunkmesh& block_chunk_mesh,blocks::block_id blkid, v3::Coord location, Dir::Dir2d face_to_attach, Dir::Dir3d direction_to_attach)
			:mesh(block_chunk_mesh), id(blkid), loc(location), face(face_to_attach), direction(direction_to_attach) {

		}
		void apply(ecs::obj& object) {
			block& blk = object.add_component<block>(mesh,loc, id, direction, face);

			BlockRegistry& registry = object.world().ensure_resource<BlockRegistry>();
			BlockTraits& traits=registry.traits[static_cast<size_t>(id)];
			if (traits.emmited_light!=0) {
				object.add_component<block_emmision>(traits.emmited_light);
			}
			if (traits.solid) {
				object.add_component<Collider>(traits.prefab.size,false,traits.solid);
			}
			
			//blk.mesh.set_face_textures()
			registry.types[static_cast<size_t>(id)]->apply(object);
		}
	};

}