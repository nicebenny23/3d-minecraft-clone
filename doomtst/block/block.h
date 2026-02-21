#include "../math/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/camera.h"
#include "../game/ecs/game_object.h"
#include "../math/geometry.h"
#include "../math/geometry.h"
#include <rpcndr.h>
#include "../game/ecs/component.h"
#include "../math/Scale3.h"
#include "../util/Option.h"
#include "../math/vector3.h"
#include "block_mesh.h"
#include "../world/chunk_mesh.h"
#include "block_registry.h"
#pragma once 
constexpr double blocksize = 1.f;

using namespace v3;
constexpr float unitaxis = 1.0f / 2.00005f;
constexpr v3::Scale3 unitscale = unit_scale * unitaxis;
namespace blocks {
	const double block_axis_scale = unitaxis * blocksize;
	const v3::Scale3 blockscale = v3::Scale3(block_axis_scale);



	struct block_emmision :ecs::component {
		block_emmision(size_t emmited_light) :emmision(emmited_light) {

		}
		size_t emmision;

	};
	struct block : ecs::component {
		blockmesh mesh;
		//based on if it is computed
		stn::Option<std::uint16_t> light_passing_through;

		v3::Coord pos;
		block_id id;
		face& operator[](size_t index) {
			return mesh[index];
		}
		bool solid() const{
			return info().solid;
		}
		Point3 center() const {
			return mesh.box.center;
		}

		Scale3 scale() const {
			return mesh.box.scale;
		}
		geo::Box bounds() const {
			return mesh.box;
		}
		template<BlockLike T>
		const bool is() const {
			return world().get_resource<BlockRegistry>().is<T>(id);
		}

		const BlockRegistry& registry() const {
			return world().get_resource<BlockRegistry>();
		}
		BlockTraits info() const{
			return registry().block_for(id)->traits();
		}
		const stn::box<BlockType>&  type() const {
			return registry().block_for(id);
		}
		block(const BlockMeshTraits& traits, Chunk::chunkmesh& chunk_mesh, v3::Coord location, blocks::block_id blockid, math::Direction3d attachment_face, math::Direction2d block_direction,bool is_solid) 
			:mesh(traits.faces, chunk_mesh.recreate_mesh, location, traits.size, attachment_face, block_direction),
			id(blockid){
			pos = location;
		};

	};

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<blocks::block> = ComponentInfo{ .updates = false };
}
//if you have a\




 // !block_H
