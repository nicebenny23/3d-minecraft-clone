#include "../math/dir.h"
#include "../renderer/renderer.h"
#include <glm/glm.hpp>
#include "../game/ecs/game_object.h"
#include "../math/geometry.h"
#include <rpcndr.h>
#include "../game/ecs/component.h"
#include "../util/Option.h"
#include "../math/vector3.h"
#include "block_mesh.h"
#include "block_registry.h"
#pragma once 
constexpr double blocksize = 1.f;

using namespace v3;
constexpr float unitaxis = 1.0f /1.00005f;
constexpr v3::Scale3 unitscale = v3::unit_scale * unitaxis;
namespace blocks {
	const double block_axis_scale = unitaxis * blocksize;
	const v3::Scale3 blockscale = v3::Scale3::from_scale(block_axis_scale);



	struct block_emmision :ecs::component {
		block_emmision(std::uint16_t emmited_light) :emmision(emmited_light) {

		}
		std::uint16_t emmision;

	};
	struct block : ecs::component {
		BlockMesh mesh;
		stn::non_null<BlockRegistry> registry;
		v3::Coord pos;
		std::uint16_t light_passing_through;

		block_id id;
		MeshFace operator[](size_t index) {
			return mesh[index];
		}
		MeshFace operator[](math::Direction3d index) {
			return mesh[index.index()];
		}
		bool solid() {
			return type().solid_traits_for().is_some();
		}
		Point3 center() const {
			return bounds().center;
		}
		geo::Box bounds() const {
			return mesh.bounds();
		}
		template<BlockLike T>
		const bool is() const {
			return world().get_resource<BlockRegistry>().is<T>(id);
		}

		
		BlockMeshTraits& mesh_info() const {
			return type().mesh_traits_for();
		}
		const BlockType&  type() const {
			return registry->get_block(id);
		}
		block(const BlockMeshTraits& traits, dirty_flag& chunk_mesh_flag, v3::Coord location, blocks::block_id block_id, math::Direction3d attachment_face, math::Direction2d block_direction,BlockRegistry& block_register) 
			:mesh(traits, chunk_mesh_flag, location, attachment_face, block_direction),id(block_id),registry(block_register),light_passing_through(0),pos(location){
			int l = 3;
		};

	};

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<blocks::block> = ComponentInfo{ .updates = false };
}
//if you have a\




 // !block_H
