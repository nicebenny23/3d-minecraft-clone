#include "../util/dynamicarray.h"
#include "geometry.h"
#pragma once
namespace math {
	const v3::Coord cube_mesh[] = {
		v3::Coord(0, 0, 0), // vertex 0
		v3::Coord(1, 0, 0), // vertex 1
		v3::Coord(1, 1, 0), // vertex 2
		v3::Coord(0, 1, 0), // vertex 3
		v3::Coord(0, 0, 1), // vertex 4
		v3::Coord(1, 0, 1), // vertex 5
		v3::Coord(1, 1, 1), // vertex 6
		v3::Coord(0, 1, 1)  // vertex 7
	};

	const v2::Vec2 square_mesh[] = {
		v2::Vec2(1, 0),
		v2::Vec2(0, 0),
		v2::Vec2(0, 1),
		v2::Vec2(1, 1),
	};
	const v2::Vec2 symetrical_square_mesh[] = {
		v2::Vec2(1, 1),
		v2::Vec2(-1,1),
		v2::Vec2(-1,-1),
		v2::Vec2(1, -1),

	};
	const int square_mesh_triangle_indices[] = {
		0, 1, 2, 0, 2, 3
	};
	//unique indices
	const int cube_mesh_face_indices[] = {
	5, 1, 2, 6,  // east (+x)
	0, 4, 7, 3,  // west (-x)
	2, 3, 7, 6,  // top (+y)
	0, 1, 5, 4,  // bottom (-y)
	4, 5, 6, 7,  // south (+z)
	0, 1, 2, 3   // north (-z)
	}; 
	const unsigned int cube_edge_indices[] = {
	0,1, 1,2, 2,3, 3,0,
	4,5, 5,6, 6,7, 7,4,
	0,4, 1,5, 2,6, 3,7
	};
}