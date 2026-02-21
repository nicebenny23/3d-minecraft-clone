#pragma once

#include <iostream>
#include "../debugger/debug.h"
#include "../math/vector2.h"

namespace texdata {
	unsigned char* texture_for(int* width, int* height, const char* name);

	v2::Coord2 image_size(const char* Img_Path);

}