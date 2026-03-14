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
		math::Direction2d face;
		math::Direction3d direction;
		Chunk::chunkmesh& mesh;
		stn::Option<stn::file_handle&> handle;
		
		//it cannot be a recipe due to its optimizations mechanics
		Chunk::block_object spawn(ecs::Ecs& world) {
			BlockRegistry& registry =world.get_resource<BlockRegistry>();
			BlockTraits traits=registry.traits_for(id);
			block* blk_ptr;
			if (traits.solid) {
				//best diffrerentiator
				blk_ptr=&world.spawn_with_component<block>(traits.mesh, mesh.recreate_mesh, loc, id, direction, face);
			}
			else {
				struct air_tag {

				};
				blk_ptr=&world.spawn_with_component_tagged<air_tag,block>(traits.mesh, mesh.recreate_mesh, loc, id, direction, face);
			}
			//stupid trick to save memory
			block& blk = *blk_ptr;
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
			return Chunk::block_object::make_unchecked(object,*blk_ptr);
		}
	};

}