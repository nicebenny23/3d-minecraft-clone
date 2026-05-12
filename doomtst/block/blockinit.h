#include "block.h"	
#include "block_registry.h"
#include "../game/aabb.h"
#include "../util/fileloader.h"
#include "../world/chunk_mesh.h"
#pragma once  
namespace blocks {


	struct GenerateBlock{
		blocks::block_id id;
		v3::Coord loc;
		math::Direction2d block_face;
		math::Direction3d direction;
		chunks::ChunkMesh& mesh;
		stn::Option<stn::file_handle&> handle;
		BlockRegistry& registry;
		//it cannot be a recipe due to its optimizations mechanics
		chunks::block_object spawn(ecs::Ecs& world) {
			BlockTraits traits=registry.traits_for(id);
			block* blk_ptr;
			using block_type= decltype(stn::make_constructor<block>(traits.mesh, mesh.recreate_mesh, loc, id, direction, block_face, registry));
			block_type block_constructor = block_type(traits.mesh, mesh.recreate_mesh, loc, id, direction, block_face, registry);
			if (!traits.solid) {
				blk_ptr=&world.spawn_with(std::move(block_constructor)).get<block&>();
			}
			else {
				blk_ptr=&world.spawn_with(std::move(block_constructor),stn::make_constructor<aabb::Collider>(!traits.solid)).get<block&>();
			}
			block& blk = *blk_ptr;
			
			ecs::obj object = blk.owner();
			ecs::entity ent_id = object.inner();
			if (traits.emmited_light!=0) {
				world.add_component_unchecked<block_emmision>(ent_id,traits.emmited_light);
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
			return chunks::block_object::make_unchecked(object,blk);
		}
	};

}