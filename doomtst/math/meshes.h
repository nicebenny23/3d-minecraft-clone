#include "../util/dynamicarray.h"
#include "geometry.h"
#pragma once
namespace math {
	const Vec3 cube_mesh[] = {
		Vec3(0, 0, 0), // vertex 0
		Vec3(1, 0, 0), // vertex 1
		Vec3(1, 1, 0), // vertex 2
		Vec3(0, 1, 0), // vertex 3
		Vec3(0, 0, 1), // vertex 4
		Vec3(1, 0, 1), // vertex 5
		Vec3(1, 1, 1), // vertex 6
		Vec3(0, 1, 1)  // vertex 7
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
}