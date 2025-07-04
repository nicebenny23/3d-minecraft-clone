
#pragma once
#include "vector3.h"
#include "List.h"
namespace Dir {

	enum class Ind3d:char {
		Right= 0,
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
		constexpr Dir3d():dir(Ind3d::None) {
		}

		// Constructor
		constexpr Dir3d(Ind3d d) noexcept : dir((d)) {}
		explicit Dir3d(char direction) {
			if (direction>=static_cast<char>(Ind3d::None)) {
				throw std::invalid_argument("Invalid Dir3d: " +((int)direction));
			}
			dir = static_cast<Ind3d>(direction);
		}
		constexpr char ind() const noexcept {
			if (dir ==Ind3d::None) {
				throw std::logic_error("Invalid dir");
			
			}
			return static_cast<char>(dir);
		}
		// Inverse direction
		 Dir3d Inv() const  {
			if (dir == Ind3d::None) {
				throw std::invalid_argument("Attempted to access invalid direction ");
			}
				return Dir3d(static_cast<Ind3d>(static_cast<char>(dir) ^ 1)); // Swaps 0↔1, 2↔3, 4↔5
			}

		// Convert to vector
		v3::Coord ToVec() const {
			switch (dir)
			{
			case Ind3d::Right:
				return  v3::Coord(1, 0, 0);//left
			case Ind3d::Left:
				return v3::Coord(-1, 0, 0);//right
			case Ind3d::Up:
				return v3::Coord(0, 1, 0);//north
			case Ind3d::Down:
				return v3::Coord(0, -1, 0);//south
			case Ind3d::Front:
				return v3::Coord(0, 0, 1);//front
			case Ind3d::Back:
				return v3::Coord(0, 0, -1);//back
			case Ind3d::None:
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
	static constexpr  Cont::List<Dir3d, 6> Directions3d= { right3d,left3d,up3d,down3d,front3d,back3d };
	enum dir2d
	{
		west2d = 0,
		east2d = 1,

		front2d = 4,
		back2d = 5,
	};
	inline Dir3d Align(v3::Vector3 point) {


		float max = Max(abs(point.x), abs(point.y), abs(point.z));
		int ind=0;
		if (abs(point.x) == max)
		{
			ind =((1 - sign(point.x)) / 2);
		}
		if (abs(point.y) == max)
		{
			ind = (2 + ((1 - sign(point.y)) / 2));
		}
		if (abs(point.z) == max)
		{
			ind = 4 + ((1 - sign(point.z)) / 2);
		}
return Dir3d(static_cast<char>(ind));
	}
	inline int max2ddirection(v3::Vector3 point) {


		float max = Max(abs(point.x), abs(point.z));
		if (abs(point.x) == max)
		{
			return (1 - sign(point.x)) / 2;
		}

		if (abs(point.z) == max)
		{
			return 4 + (1 - sign(point.z)) / 2;
		}
	}
}