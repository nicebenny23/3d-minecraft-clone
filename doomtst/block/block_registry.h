#include "../game/ecs/ecs.h"
#include "../math/Scale3.h"
#include "../util/List.h"
#include "../util/fileloader.h"
#pragma once
namespace blocks {
	//todo replace
	struct block_tag {

	};

	using block_texture = std::uint8_t;
	struct BlockTextureRegistry:ecs::resource{
		std::unordered_map<std::string, block_texture> name_to_texture;
		size_t last_saved;
		BlockTextureRegistry() :last_saved(0) {
			name_to_texture.reserve(32);
		}
		bool need_sync() const {
			return last_saved != name_to_texture.size();
		}
		
		block_texture get_texture(std::string_view name) {
			 return name_to_texture.try_emplace(std::string(name), block_texture(name_to_texture.size())).second;
		} 
	};
	struct block_tag;

	using block_id = stn::typed_id<block_tag,std::uint16_t>;

	struct block;
	struct BlockMeshTraits {
		BlockMeshTraits(v3::Scale3 mesh_size, bool is_transparent, block_texture left_face, block_texture right_face, block_texture up_face, block_texture down_face, block_texture front_face, block_texture back_face,bool is_invisible=false)
			:faces(left_face, right_face, up_face, down_face, front_face, back_face), 
			size(mesh_size),
			transparent(is_transparent),
			invisible(is_invisible)
		{
		}
		BlockMeshTraits(v3::Scale3 mesh_size, bool is_transparent, block_texture only_texture, bool is_invisible = false)
			:faces(only_texture, only_texture, only_texture, only_texture, only_texture, only_texture),
			size(mesh_size),
			transparent(is_transparent),
			invisible(is_invisible) {   
		}
		stn::List<block_texture, 6> faces;
		bool transparent;
		bool invisible = false;
		v3::Scale3 size = v3::unit_scale;
	};
	struct SolidBlockTraits {
		size_t time_to_mine = 1;
		size_t power_level=0;
		bool pick_speedup=true;
		SolidBlockTraits(size_t mining_level):time_to_mine(mining_level),power_level(time_to_mine) {

		}
		SolidBlockTraits() {
			
		}
		SolidBlockTraits(size_t mining_time,size_t power,bool pick_speedup=true) :time_to_mine(mining_time), power_level(power), pick_speedup(pick_speedup){

		}

	};
	inline constexpr bool solid_block = true;
	inline constexpr bool non_solid_block = false;

	struct BlockTraits {
		BlockTraits(BlockMeshTraits BlockMesh, bool is_solid = true, size_t light_emmision = 0)
			:mesh(BlockMesh), solid(is_solid), emmited_light(light_emmision) {
		}
		BlockMeshTraits mesh;
		size_t emmited_light;
		bool solid;
		BlockTraits() = default;
	};
	struct BlockType {
		virtual ~BlockType() = default;
		BlockType() = default;
		BlockType(ecs::Ecs& blocks) {
		};
		virtual std::string name() const = 0;
		virtual SolidBlockTraits mining_traits() const {
			return SolidBlockTraits();
		}
		//we catch the traits to speed up;
		BlockTraits& traits_for(BlockTextureRegistry& textures) const {
			if (!catched) {
				catched = traits(textures);
			}
			return catched.unwrap_unchecked();
		}
		virtual BlockTraits traits(BlockTextureRegistry& textures) const = 0;
		virtual void apply(ecs::obj& blk) const {

		};
		//this sucks but i need to simplify my components to remove it
		virtual void read_from_bytes(ecs::obj blk, stn::file_handle& handle) const {
			apply(blk);
		};
		virtual void write_to_bytes(ecs::obj blk, stn::file_handle& handle) const {
			
		};
	private:
		mutable stn::Option<BlockTraits> catched;

	};
	//you have to both write and read to bytes
	template<typename T>
	concept BlockLike =std::derived_from<T, BlockType> &&
	!((& T::read_from_bytes != &BlockType::read_from_bytes)^(& T::write_to_bytes != &BlockType::write_to_bytes));
	enum BlockRecipeModes {
		serialize,
		no_serialize,
		none,
	};

	struct BlockRegistry :ecs::resource {
		stn::array<stn::Option<size_t>> to_id;
		BlockTraits& traits_for(block_id id) {
			return blocks[id.id]->traits_for(textures);
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
		block_id insert() {
			return ids.insert<T>().on_insert([&](block_id id) {
				if constexpr (std::constructible_from<T,ecs::Ecs&>) {
					blocks.emplace(stn::construct_derived<T>(), ecs);
				}
				else {

					blocks.emplace(stn::construct_derived<T>());
				}
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
					return block_id(std::uint16_t(i));
				}
			}
			stn::throw_logic_error("block with name {} does not exist", name);
		}
		void apply(ecs::obj& blk, block_id id) {
			return blocks[id.id]->apply(blk);
		}
		BlockRegistry(ecs::Ecs& world) :ecs(world) {

		}
		BlockTextureRegistry textures;
	private:
		ecs::Ecs& ecs;
		stn::type_indexer<block_id> ids;

		stn::array<stn::box<BlockType>> blocks;
	};
};