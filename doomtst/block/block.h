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
#include "block_registry.h"
#pragma once 
constexpr double blocksize = 1.f;

using namespace v3;
constexpr float unitaxis = 1.0f /1.00005f;
constexpr v3::Scale3 unitscale = unit_scale * unitaxis;
namespace blocks {
	const double block_axis_scale = unitaxis * blocksize;
	const v3::Scale3 blockscale = v3::Scale3(block_axis_scale);



	struct block_emmision :ecs::component {
		block_emmision(std::uint16_t emmited_light) :emmision(emmited_light) {

		}
		std::uint16_t emmision;

	};
	struct block : ecs::component {
		block_mesh mesh;
		v3::Coord pos;
		std::uint16_t light_passing_through;

		block_id id;
		MeshFace operator[](size_t index) {
			return mesh[index];
		}
		MeshFace operator[](math::Direction3d index) {
			return mesh[index.index()];
		}
		bool solid() const{
			return info().solid;
		}
		Point3 center() const {
			return bounds().center;
		}
		math::Box bounds() const {
			return mesh.bounds();
		}
		template<BlockLike T>
		const bool is() const {
			return world().get_resource<BlockRegistry>().is<T>(id);
		}

		const BlockRegistry& registry() const {
			return world().get_resource<BlockRegistry>();
		}
		BlockTraits& info() const{
			return registry().traits_for(id);
		}
		const stn::box<BlockType>&  type() const {
			return registry().block_for(id);
		}
		block(const BlockMeshTraits& traits, dirty_flag& chunk_mesh_flag, v3::Coord location, blocks::block_id block_id, math::Direction3d attachment_face, math::Direction2d block_direction) 
			:mesh(traits, chunk_mesh_flag, location, attachment_face, block_direction),
			id(block_id){
			light_passing_through = 0;
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
