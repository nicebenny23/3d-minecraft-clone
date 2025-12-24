
#pragma once
#include "vector3.h"
#include "../util/List.h"
namespace Dir {

	enum class Ind3d :char {
		Right = 0,
		Left = 1,
		Up = 2,
		Down = 3,
		Front = 4,
		Back = 5,
		None = 6
	};



	struct Dir3d {

	private:
		Ind3d dir;

	public:
		// Direction constants
		constexpr Dir3d() :dir(Ind3d::None) {
		}

		// Constructor
		constexpr Dir3d(Ind3d d) noexcept : dir((d)) {
		}
		explicit Dir3d(char direction) {
			if (direction >= static_cast<char>(Ind3d::None)) {
				throw std::invalid_argument("Invalid Dir3d: " + ((int)direction));
			}
			dir = static_cast<Ind3d>(direction);
		}
		constexpr char ind() const {
			if (dir == Ind3d::None) {
				throw std::logic_error("Invalid dir");

			}
			return static_cast<char>(dir);
		}
		constexpr size_t axis_index() const {
			if (dir == Ind3d::None) {
				throw std::logic_error("Invalid dir");

			}
			return static_cast<char>(dir)/2;
		}
		// Inverse direction
		Dir3d Inv() const {
			if (dir == Ind3d::None) {
				throw std::invalid_argument("Attempted to access invalid direction ");
			}
			return Dir3d(static_cast<Ind3d>(static_cast<char>(dir) ^ 1)); // Swaps 0↔1, 2↔3, 4↔5
		}

		// Convert to vector
		v3::Coord to_coord() const {
			switch (dir) {
			case Ind3d::Right:
			return  v3::RightCoord;//right
			case Ind3d::Left:
			return v3::LeftCoord;//left
			case Ind3d::Up:
			return v3::UpCoord;//north
			case Ind3d::Down:
			return v3::DownCoord;//south
			case Ind3d::Front:
			return v3::FrontCoord;//front
			case Ind3d::Back:
			return v3::BackCoord;//back
			default:
			throw std::invalid_argument("Attempted to access invalid direction");
			break;


			}
		}
		v3::Vec3 to_vec() const {
			switch (dir) {
			case Ind3d::Right:
			return  v3::right;//right
			case Ind3d::Left:
			return v3::left;//left
			case Ind3d::Up:
			return v3::up;//north
			case Ind3d::Down:
			return v3::down;//south
			case Ind3d::Front:
			return v3::forward;//front
			case Ind3d::Back:
			return v3::back;//back
			default:
			throw std::invalid_argument("Attempted to access invalid direction");
			break;


			}
		}
		constexpr bool operator==(const Dir3d& other) const noexcept {
			return dir == other.dir;
		}

		constexpr bool operator!=(const Dir3d& other) const noexcept {
			return !(*this == other);
		}
	};
	static constexpr Dir3d right3d = Dir3d(Ind3d::Right);  // (1, 0, 0)
	static constexpr Dir3d  left3d = Dir3d(Ind3d::Left); // (-1, 0, 0)
	static constexpr Dir3d up3d = Dir3d(Ind3d::Up);    // (0, 1, 0)
	static constexpr Dir3d down3d = Dir3d(Ind3d::Down);   // (0, -1, 0)
	static constexpr Dir3d front3d = Dir3d(Ind3d::Front);  // (0, 0, 1)
	static constexpr Dir3d  back3d = Dir3d(Ind3d::Back);   // (0, 0, -1)
	static constexpr Dir3d  None3d = Dir3d(Ind3d::None);   // (0, 0, -1)
	static constexpr  stn::List<Dir3d, 6> Directions3d = { right3d,left3d,up3d,down3d,front3d,back3d };
	inline Dir3d Align(v3::Vec3 point) {


		double max = Max(abs(point.x), abs(point.y), abs(point.z));
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
		return Dir3d(static_cast<char>(ind));
	}

	inline int max2ddirection(v3::Vec3 point) {


		double max = Max(abs(point.x), abs(point.z));
		if (abs(point.x) == max) {
			return (1 - sign(point.x)) / 2;
		}

		if (abs(point.z) == max) {
			return 2 + (1 - sign(point.z)) / 2;
		}
		throw std::logic_error("work");
	}

	enum class Ind2d : char {
		Right = 0,
		Left = 1,
		Up = 2,
		Down = 3,
		None = 4
	};

	struct Dir2d {
	private:
		Ind2d dir;

	public:
		// Default constructor (None)
		constexpr Dir2d() : dir(Ind2d::None) {
		}

		// Constructor from enum
		constexpr Dir2d(Ind2d d) noexcept : dir(d) {
		}

		// Constructor from char with validation
		explicit Dir2d(char direction) {
			if (direction >= static_cast<char>(Ind2d::None)) {
				throw std::invalid_argument("Invalid Dir2d: " + ((int)direction));
			}
			dir = static_cast<Ind2d>(direction);
		}

		// Get underlying char index
		constexpr char ind() const {
			if (dir == Ind2d::None) {
				throw std::logic_error("Invalid direction");
			}
			return static_cast<char>(dir);
		}

		Dir2d Inv() const {
			if (dir == Ind2d::None) {
				throw std::invalid_argument("Attempted to access invalid direction");
			}
			return Dir2d(static_cast<Ind2d>(static_cast<char>(dir) ^ 1));
		}

		// Convert to vector (assuming v2::Coord exists similar to v3::Coord)
		v2::Coord2 to_coord() const {
			switch (dir) {
			case Ind2d::Right:
			return v2::Coord2(1, 0);  // (1, 0)
			case Ind2d::Left:
			return v2::Coord2(-1, 0);   // (-1, 0)
			case Ind2d::Up:
			return v2::Coord2(0, 1);     // (0, 1)
			case Ind2d::Down:
			return v2::Coord2(0, -1);   // (0, -1)
			case Ind2d::None:
			throw std::invalid_argument("Attempted to access invalid direction");
			}
			// Just to suppress compiler warnings (unreachable)
			return v2::Coord2(0, 0);
		}

		constexpr bool operator==(const Dir2d& other) const noexcept {
			return dir == other.dir;
		}

		constexpr bool operator!=(const Dir2d& other) const noexcept {
			return !(*this == other);
		}
	};

	// Direction constants
	static constexpr Dir2d right2d = Dir2d(Ind2d::Right);  // (1, 0)
	static constexpr Dir2d left2d = Dir2d(Ind2d::Left);    // (-1, 0)
	static constexpr Dir2d up2d = Dir2d(Ind2d::Up);        // (0, 1)
	static constexpr Dir2d down2d = Dir2d(Ind2d::Down);    // (0, -1)
	static constexpr Dir2d None2d = Dir2d(Ind2d::None);    // none

	static constexpr stn::List<Dir2d, 4> Directions2d = { right2d, left2d, up2d, down2d };

	inline Dir2d Align2d(v2::Vec2 point) {
		double max_val = Max(abs(point.x), abs(point.y));
		int ind = 0;

		if (abs(point.x) == max_val) {
			ind = ((1 - sign(point.x)) / 2); // 0 if positive (Right), 1 if negative (Left)
		}
		if (abs(point.y) == max_val) {
			ind = 2 + ((1 - sign(point.y)) / 2); // 2 if positive (Up), 3 if negative (Down)
		}
		return Dir2d(static_cast<char>(ind));
	}
}