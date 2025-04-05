#include "../external/stb_image.h"
#include <iostream>
#include "../debugger/debug.h"
#pragma once

namespace texdata{
inline unsigned char* loadtexdata(int* width, int* height, const char* name) {
	int colchannel = 0;
	unsigned char* data = stbi_load(name, width, height, &colchannel, STBI_rgb_alpha);

	if (data == nullptr)
	{
		std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
		Assert("texture doesent exist");
	}
	return data;
}
}