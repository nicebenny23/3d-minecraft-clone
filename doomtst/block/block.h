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




	struct blockatt {
		bool solid;
		blockatt() {
			solid = true;
		}
	};
	struct block_emmision :ecs::component {
		block_emmision(size_t emmited_light) :emmision(emmited_light) {

		}
		size_t emmision;

	};
	struct block : ecs::component {
		blockmesh mesh;
		v3::Coord pos;
		//based on if it is computed
		stn::Option<size_t> light_passing_through;
		block_id id;
		face& operator[](size_t index) {
			return mesh[index];
		}
		blockatt attributes;
		char mininglevel;
		bool minedfastwithpick;
		Point3 center() const {
			return mesh.box.center;
		}

		Scale3 scale() const {
			return mesh.box.scale;
		}
		geo::Box bounds() const {
			return mesh.box;
		}

		block(const BlockMeshTraits& traits, Chunk::chunkmesh& chunk_mesh, v3::Coord location, blocks::block_id blockid, math::Direction3d attachment_face, math::Direction2d block_direction,bool is_solid,SolidBlockTraits sb_traits) 
			:mesh(traits.faces, chunk_mesh.recreate_mesh, location, traits.size, attachment_face, block_direction) {
			mininglevel = 0;
			id = blockid;
			mininglevel = static_cast<char>(sb_traits.time_to_mine);
			pos = location;
			attributes.solid = is_solid;
		};
		void createdefaultaabb(bool effector = false);

	};

}
namespace ecs {
	template<>
	inline constexpr ComponentInfo ComponentTraits<blocks::block> = ComponentInfo{ .updates = false };
}
//if you have a\




 // !block_H
