#pragma once

#include "../external/stb_image.h"
#include <iostream>
#include "../debugger/debug.h"
#include "../math/vector2.h"

namespace texdata {
	inline unsigned char* loadtexdata(int* width, int* height, const char* name) {
		int colchannel = 0;
		unsigned char* data = stbi_load(name, width, height, &colchannel, STBI_rgb_alpha);

		if (data == nullptr)
		{
			std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
			throw std::invalid_argument(std::string("Failed to load image: ") + name);
		}
		return data;
	}
inline v2::Coord2 GetImgSize(const char* Img_Path) {
		int channels;
		v2::Coord2 size;
		// Load image to get its info
		if (!stbi_info(Img_Path, &size.x, &size.y, &channels)) {
			throw std::invalid_argument(std::string("Failed to load image: ")+Img_Path);
		}
		return size;
	}
}