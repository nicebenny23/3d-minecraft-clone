#pragma once

#include <iostream>
#include "../debugger/debug.h"
#include "../math/vector2.h"

namespace texdata {
	unsigned char* loadtexdata(int* width, int* height, const char* name);

	v2::Coord2 GetImgSize(const char* Img_Path);

}