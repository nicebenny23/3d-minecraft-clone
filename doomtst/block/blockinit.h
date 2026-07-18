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
			const BlockType& type =registry.get_block(id);
			block* blk_ptr;
			using block_type= decltype(stn::make_constructor<block>(type.mesh_traits_for(), mesh.recreate_mesh, loc, id, direction, block_face, registry));
			block_type block_constructor = block_type(type.mesh_traits_for(), mesh.recreate_mesh, loc, id, direction, block_face, registry);
			ecs::entity ent_id;

			if (!type.solid_traits_for().is_some()) {
				auto idk = world.spawn_with(std::move(block_constructor));
				ent_id = idk.get<ecs::entity>();
				blk_ptr = &idk.get<block&>();
			}
			else {

				auto idk = world.spawn_with(std::move(block_constructor), stn::make_constructor<aabb::Collider>(false));
				ent_id = idk.get<ecs::entity>();

				blk_ptr=&idk.get<block&>();
			}
			block& blk = *blk_ptr;
			ecs::obj object{ ent_id,world };
			if (type.emmited_light()!= 0) {
				world.add_component_unchecked<block_emmision>(ent_id,type.emmited_light());
			}
			if (type.mesh_traits_for().transparent) {
				blk.mesh.transparent = true;
			}
			if (handle) {
				type.read_from_bytes(object, handle.unwrap());
			}
			else {
				type.apply(object);
			}
			return chunks::block_object::make_unchecked(object,blk);
		}
	};

}