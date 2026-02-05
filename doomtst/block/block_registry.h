#include "../game/ecs/ecs.h"
#include "block_mesh.h"
#pragma once
namespace blocks {
	struct block;
	struct BlockMeshTraits {
		constexpr BlockMeshTraits(v3::Scale3 mesh_size, bool is_transparent, block_textures left_face, block_textures right_face, block_textures up_face, block_textures down_face, block_textures front_face, block_textures back_face)
			:faces(left_face, right_face, up_face, down_face, front_face, back_face),size(mesh_size),transparent(is_transparent) {

		}
		stn::List<block_textures, 6> faces;
		bool transparent;
		v3::Scale3 size=v3::unit_scale;
	};
	struct SolidBlockTraits {
		size_t time_to_mine=1;

	};
	inline constexpr bool solid_block = true;
	inline constexpr bool non_solid_block = false;

	struct BlockTraits {
		constexpr BlockTraits(BlockMeshTraits block_mesh, bool is_solid = true, size_t light_emmision = 0)
		:mesh(block_mesh),solid(is_solid),emmited_light(light_emmision){
		}
		BlockMeshTraits mesh;
		size_t emmited_light;
		bool solid;
		BlockTraits() = default;
	};
	struct BlockType {
		virtual ~BlockType() = default;
		BlockType() = default;

		virtual SolidBlockTraits mining_traits() const {
			return SolidBlockTraits();
		}
		virtual void apply(ecs::obj& blk) {

		};
	};
	
	template<typename T>
	inline constexpr BlockTraits BlockInfo{};
	struct BlockRegistry :ecs::resource {
		stn::array<stn::box<BlockType>> types;
		stn::array<BlockTraits> traits;
		stn::array<stn::Option<size_t>> to_id;
		template<typename T>
		void register_block(block_id id_for) {
			types.emplace(stn::construct_derived<T>());
			traits.push(BlockInfo<T>);
			to_id.reach(static_cast<size_t>(id_for)) = traits.last_index();
		}
		BlockTraits& traits_for(block_id id) {
			return traits[to_id[id].expect("block should exist")];
		}
		BlockType& type_for(block_id id) {
			return *types[to_id[id].expect("block should exist")];
		}

	};
};