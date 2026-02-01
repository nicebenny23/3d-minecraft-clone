#include "../game/ecs/ecs.h"
#include "block_mesh.h"
#pragma once
namespace blocks {
	struct block;
	struct BlockMeshTraits {
		block_textures faces[6];
		bool transparent=false;
		v3::Scale3 size=v3::unit_scale;

		BlockMeshTraits() = default;
	};
	struct BlockTraits {
		BlockMeshTraits prefab;
		size_t emmited_light=0;
		bool solid=false;
		BlockTraits() = default;
	};
	struct BlockType {
		virtual void apply(ecs::obj& blk) {

		};
	};
	
	template<typename T>
	inline constexpr BlockTraits BlockInfo{ };
	struct BlockRegistry :ecs::resource {
		stn::array<stn::box<BlockType>> types;
		stn::array<BlockTraits> traits;
	
		template<typename T>
		void register_block(block_id id_for) {
			types.emplace(stn::construct_derived<T>());
			traits.push(BlockInfo<T>);
		}


	};
};