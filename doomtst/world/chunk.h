#include "../block/block.h"
#include "../util/cached.h"
#include "../game/ecs/filtered_object.h"
#include "chunk_mesh.h"
#include "../math/dir.h"
#pragma once 
using namespace blocks;
namespace Chunk {


	inline std::string file_name(ChunkLocation pos, grid::world& world) {
		std::string m = std::format("Chunk{}", pos.position);
		std::filesystem::path path = world.get_path();
		path = path / "Chunks" / m;
		return 	path.string();
	}
	inline size_t index_from_pos(Coord pos) {
		int x = symmetric_mod(pos.x, chunk_axis);
		int y = symmetric_mod(pos.y, chunk_axis);
		int z = symmetric_mod(pos.z, chunk_axis);
		return	chunk_axis * chunk_axis * x + chunk_axis * y + z;
	}
	inline size_t index_from_local_position(size_t x, size_t y, size_t z) {
		return	chunk_axis * chunk_axis * x + chunk_axis * y + z;
	}
	using block_object = ecs::Constrained<block>;
	struct chunk :ecs::component {
		stn::array<ecs::Constrained<block>> block_list;
		Chunk::ChunkLocation location;
		bool modified;
		chunk(Chunk::ChunkLocation chunk_location) :modified(false), location(chunk_location), block_list() {
			block_list.reserve(chunk_elements);
		}

		Point3 center() const {
			return location.center();
		}
		geo::Box bounds() const {
			return location.bounds();
		}
		ecs::Constrained<block>& operator[](size_t index) {
			return block_list[index];
		}
		const ecs::Constrained<block>& operator[](size_t index) const {
			return block_list[index];
		}

		using iterator = decltype(block_list)::iterator;
		using const_iterator = decltype(block_list)::const_iterator;
		iterator begin() {
			return block_list.begin();
		}
		iterator end() {
			return block_list.end();
		}
		const_iterator begin() const {
			return block_list.begin();
		}
		const_iterator end() const {
			return block_list.end();
		}
		//A range on the a Face(16-16) region a chunk 
		template<typename T> requires std::same_as<std::remove_const_t<T>,chunk>
		struct FaceRange {
			size_t face_index(size_t x, size_t y) const {
				switch (dir.direction()) {
				case math::DirectionIndex3d::Right: return index_from_local_position(chunk_axis - 1, x, y);
				case math::DirectionIndex3d::Left: return index_from_local_position(0, x, y);
				case math::DirectionIndex3d::Up: return index_from_local_position(x, chunk_axis - 1, y);
				case math::DirectionIndex3d::Down: return index_from_local_position(x, 0, y);
				case math::DirectionIndex3d::Front: return index_from_local_position(x, y, chunk_axis - 1);
				case math::DirectionIndex3d::Back: return index_from_local_position(x, y, 0);
				}
			}
			struct iterator {
				size_t i;
				size_t j;
				FaceRange& range;
				iterator& operator++() {
					++i;
					if (i >= chunk_axis) {
						i = 0;
						++j;
					}
					return *this;
				}
				bool operator==(const iterator& other) const{
					return i == other.i && j == other.j && &range == &other.range;
				}
				bool operator!=(const iterator& other) const {
					return !(*this==other);
				}
				ecs::Constrained<block>& operator*() {
					return range.chnk.block_list[range.face_index(i, j)];
				}
			
			};


			iterator begin() {
				return iterator{ .i = 0,.j = 0,.range = *this };
			}
			iterator end() {
				return iterator{ .i = 0,.j = chunk_axis,.range = *this };
			}
		private:
			friend struct iterator;

			friend struct chunk;
			FaceRange(T& chunk, math::Direction3d direction) :dir(direction), chnk(chunk) {

			}
			T& chnk;
			math::Direction3d dir;
		};

		using face_range = FaceRange<chunk>;
		using const_face_range = FaceRange<const chunk>;
		//returns the blocks on the 16-16 face matching the direction
		face_range on_face(math::Direction3d direction) {
			return face_range(*this, direction);
		}

		//returns the blocks on the 16-16 face matching the direction
		const_face_range on_face(math::Direction3d direction) const{
			return const_face_range(*this, direction);
		}
		void destroy_hook() {
			if (modified) {
				file_handle file = file_handle(file_name(location.position, world().get_resource<grid::world>()), FileMode::ReadWriteBinary);
				for (int i = 0; i < chunk_elements; i++) {
					block& block_at = block_list[i].get<block>();
					stn::file_serializer<block_id>().write(block_at.id, file);
					stn::file_serializer<math::Direction2d>().write(block_at.mesh.direction, file);
					stn::file_serializer<math::Direction3d>().write(block_at.mesh.attached_direction, file);
					block_at.type()->write_into_bytes(block_list[i].object(), file);
				}
				file.close();
			}
			for (int i = 0; i < chunk_elements; i++) {
				std::move(block_list[i]).destroy();
			}
		}
	};


}

// !Chunk_H
#pragma once
