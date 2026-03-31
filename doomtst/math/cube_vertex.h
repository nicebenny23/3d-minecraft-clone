#include "vector3.h"
#include "dir.h"
#pragma once
namespace math{
	struct cube_index {
		constexpr explicit cube_index(std::uint8_t vertex):index(vertex) {

		}
		//returns the cube vertex associated with this index
		constexpr v3::Coord vertex() const {
			return v3::Coord(index & 1, (index >> 1) & 1, (index >> 2) & 1);
		}
		constexpr v3::Vec3 vertex_vector() const {
			return v3::Vec3(index & 1, (index >> 1) & 1, (index >> 2) & 1);
		}
		constexpr std::uint8_t ind() const {
			return index;
		}
		static stn::Option<cube_index> from_coord(const v3::Coord& c) {
			if ((c.x > 1) || (c.y > 1) || (c.z > 1)) {
				return stn::None;
			}
			if ((c.x<0) || (c.y<0) || (c.z<0)) {
				return stn::None;
			}
			return cube_index((c.z << 2) | (c.y << 1) | c.x);
		}
	private:
		std::uint8_t index;
	};
	constexpr cube_index cube_indices[8] = { cube_index(0), cube_index(1),cube_index( 2), cube_index(3),cube_index( 4), cube_index(5), cube_index(6), cube_index(7) };
}