#include "../game/ecs/ecs.h"
#include "block_mesh.h"
#pragma once
namespace blocks {
	struct block;
	struct BlockMeshTraits {
		constexpr BlockMeshTraits(v3::Scale3 mesh_size, bool is_transparent, block_textures left_face, block_textures right_face, block_textures up_face, block_textures down_face, block_textures front_face, block_textures back_face)
			:faces(left_face, right_face, up_face, down_face, front_face, back_face), size(mesh_size), transparent(is_transparent) {

		}
		stn::List<block_textures, 6> faces;
		bool transparent;
		v3::Scale3 size = v3::unit_scale;
	};
	struct SolidBlockTraits {
		size_t time_to_mine = 1;

	};
	inline constexpr bool solid_block = true;
	inline constexpr bool non_solid_block = false;

	struct BlockTraits {
		constexpr BlockTraits(BlockMeshTraits block_mesh, bool is_solid = true, size_t light_emmision = 0)
			:mesh(block_mesh), solid(is_solid), emmited_light(light_emmision) {
		}
		BlockMeshTraits mesh;
		size_t emmited_light;
		bool solid;
		BlockTraits() = default;
	};
	struct BlockType {
		virtual ~BlockType() = default;
		BlockType() = default;
		virtual std::string name() const = 0;
		virtual SolidBlockTraits mining_traits() const {
			return SolidBlockTraits();
		}
		virtual BlockTraits traits() const= 0;
		virtual void apply(ecs::obj& blk) const {

		};
		//this sucks but i need to simplify my components to remove it
		virtual void read_from_bytes(ecs::obj blk,stn::file_handle& handle) const {

		};
		virtual void write_into_bytes(ecs::obj blk, stn::file_handle& handle) const {

		};
	};
	template<typename T>
	concept BlockLike = std::derived_from<T, BlockType>;


	struct BlockRegistry :ecs::resource {
		stn::array<stn::Option<size_t>> to_id;
		BlockTraits traits_for(block_id id) {
			return blocks[id.id]->traits();
		}
		SolidBlockTraits solid_traits_for(block_id id) {
			return blocks[id.id]->mining_traits();
		}
		std::string name_for(block_id id) {
			return blocks[id.id]->name();
		}
		template<BlockLike T>
		bool is(block_id id) const {	
			return ids.get_opt<T>() == id;
		}
		template<BlockLike T>
		block_id register_block() {
			return ids.insert<T>().on_insert([&](block_id id) {
				blocks.emplace(stn::construct_derived<T>());
				}).value;
		}
		template<BlockLike T>
		block_id get_id() const {
			return ids.get<T>();
		}

		const stn::box<BlockType>& block_for(block_id id) const {
			return blocks[id.id];
		}
		block_id get_id(std::string_view name) const {
			for (size_t i = 0; i < blocks.length();i++) {
				if (name==blocks[i]->name()) {
					return block_id(i);
				}
			}
			stn::throw_logic_error("block with name {} does not exist", name);
		}
		void apply(ecs::obj& blk, block_id id) {
			return blocks[id.id]->apply(blk);
		}
		BlockRegistry(ecs::Ecs& world) :ecs(world) {

		}

	private:
		ecs::Ecs& ecs;
		stn::type_indexer<block_id> ids;

		stn::array<stn::box<BlockType>> blocks;
	};
};