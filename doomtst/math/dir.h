
#pragma once
#include "vector3.h"
#include "../util/List.h"
namespace math {

	enum class DirectionIndex3d :char {
		Right = 0,
		Left = 1,
		Up = 2,
		Down = 3,
		Front = 4,
		Back = 5
	};
	enum class AxisIndex3d :char {
		Right = 0,
		Up = 1,
		Front = 2,
	};

	inline constexpr AxisIndex3d Axis3d[3] = { AxisIndex3d::Right, AxisIndex3d::Up, AxisIndex3d::Front };
	struct Direction3d {


	public:
		constexpr Direction3d(DirectionIndex3d d) : dir(d) {
		}

		constexpr size_t index() const {
			return static_cast<size_t>(dir);
		}
		constexpr size_t inverse_index() const {

			return static_cast<size_t>(dir) ^ 1;
		}
		constexpr AxisIndex3d axis() const {
			return AxisIndex3d(static_cast<size_t>(dir) / 2);
		}

		Direction3d operator-() const {
			return Direction3d(static_cast<DirectionIndex3d>(static_cast<size_t>(dir) ^ 1));
		}

		// Convert to vector
		v3::Coord coord() const {
			switch (dir) {
			case DirectionIndex3d::Right:
			return  v3::RightCoord;//right
			case DirectionIndex3d::Left:
			return v3::LeftCoord;//left
			case DirectionIndex3d::Up:
			return v3::UpCoord;//north
			case DirectionIndex3d::Down:
			return v3::DownCoord;//south
			case DirectionIndex3d::Front:
			return v3::FrontCoord;//front
			case DirectionIndex3d::Back:
			return v3::BackCoord;//back
			default:
			throw std::invalid_argument("Attempted to access invalid direction");
			break;


			}
		}
		v3::Vec3 vec() const {
			switch (dir) {
			case DirectionIndex3d::Right:
			return  v3::right;//right
			case DirectionIndex3d::Left:
			return v3::left;//left
			case DirectionIndex3d::Up:
			return v3::up;//north
			case DirectionIndex3d::Down:
			return v3::down;//south
			case DirectionIndex3d::Front:
			return v3::forward;//front
			case DirectionIndex3d::Back:
			return v3::back;//back
			default:
			throw std::invalid_argument("Attempted to access invalid direction");
			break;

			}
		}
		constexpr bool operator==(const Direction3d& other) const noexcept {
			return dir == other.dir;
		}

		constexpr bool operator!=(const Direction3d& other) const noexcept {
			return !(*this == other);
		}
	private:
		DirectionIndex3d dir;

	};
	inline constexpr Direction3d right_3d = Direction3d(DirectionIndex3d::Right);  // (1, 0, 0)
	inline constexpr Direction3d  left_3d = Direction3d(DirectionIndex3d::Left); // (-1, 0, 0)
	inline constexpr Direction3d up_3d = Direction3d(DirectionIndex3d::Up);    // (0, 1, 0)
	inline constexpr Direction3d down_3d = Direction3d(DirectionIndex3d::Down);   // (0, -1, 0)
	inline constexpr Direction3d front_3d = Direction3d(DirectionIndex3d::Front);  // (0, 0, 1)
	inline constexpr Direction3d  back_3d = Direction3d(DirectionIndex3d::Back);   // (0, 0, -1)


	inline constexpr  stn::List<Direction3d, 6> Directions3d(right_3d, left_3d, up_3d, down_3d, front_3d, back_3d);
	inline Direction3d Align(v3::Vec3 point) {


		double max = stn::Max(abs(point.x), abs(point.y), abs(point.z));
		int ind = 0;
		if (abs(point.x) == max) {
			ind = ((1 - sign(point.x)) / 2);
		}
		if (abs(point.y) == max) {
			ind = (2 + ((1 - sign(point.y)) / 2));
		}
		if (abs(point.z) == max) {
			ind = 4 + ((1 - sign(point.z)) / 2);
		}
		return Direction3d(static_cast<DirectionIndex3d>(ind));
	}

	enum class AxisIndex2d :char {
		Right = 0,
		Up = 1
	};
	enum class DirectionIndex2d : std::uint8_t {
		Right = 0,
		Left = 1,
		Up = 2,
		Down = 3
	};

	struct Direction2d {
	private:
		DirectionIndex2d dir;

	public:

		constexpr Direction2d(DirectionIndex2d d) noexcept : dir(d) {
		}
		constexpr size_t index() const {
			return static_cast<size_t>(dir);
		}

		Direction2d operator-() const {
			return Direction2d(static_cast<DirectionIndex2d>(static_cast<size_t>(dir) ^ 1));
		}


		v2::Coord2 coord() const {
			switch (dir) {
			case DirectionIndex2d::Right:
			return v2::Coord2(1, 0);
			case DirectionIndex2d::Left:
			return v2::Coord2(-1, 0);
			case DirectionIndex2d::Up:
			return v2::Coord2(0, 1);
			case DirectionIndex2d::Down:
			return v2::Coord2(0, -1);
			}
		}

		constexpr bool operator==(const Direction2d& other) const = default;

		constexpr bool operator!=(const Direction2d& other) const noexcept = default;
	};

	inline Direction2d max_2d_direction(v3::Vec3 point) {


		double max = stn::Max(abs(point.x), abs(point.z));
		size_t index;
		if (abs(point.x) == max) {
			index = (1 - sign(point.x)) / 2;
		}
		else{
			index = 2 + (1 - sign(point.z)) / 2;
		}
		return Direction2d(static_cast<DirectionIndex2d>(index));
	}
	// Direction constants
	static constexpr Direction2d right2d = Direction2d(DirectionIndex2d::Right);  // (1, 0)
	static constexpr Direction2d left2d = Direction2d(DirectionIndex2d::Left);    // (-1, 0)
	static constexpr Direction2d up2d = Direction2d(DirectionIndex2d::Up);        // (0, 1)
	static constexpr Direction2d down2d = Direction2d(DirectionIndex2d::Down);    // (0, -1)

	static constexpr stn::List<Direction2d, 4> Directions2d(right2d, left2d, up2d, down2d);

	inline Direction2d Align2d(v2::Vec2 point) {
		double max_val = stn::Max(abs(point.x), abs(point.y));
		size_t ind = 0;

		if (abs(point.x) == max_val) {
			ind = ((1 - sign(point.x)) / 2);
		}
		if (abs(point.y) == max_val) {
			ind = 2 + ((1 - sign(point.y)) / 2);
		}
		return Direction2d(static_cast<DirectionIndex2d>(ind));
	}
}