#include "stbiload.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../external/stb_image.h"


unsigned char* texdata::loadtexdata(int* width, int* height, const char* name) {
	int colchannel = 0;
	unsigned char* data = stbi_load(name, width, height, &colchannel, STBI_rgb_alpha);

	if (data == nullptr) {
		std::cerr << "Failed to load texture: " << stbi_failure_reason() << std::endl;
		throw std::invalid_argument(std::string("Failed to load image: ") + name);
	}
	return data;

}

v2::Coord2 texdata::GetImgSize(const char* Img_Path) {
	int channels;
	v2::Coord2 size;
	// Load image to get its info
	if (!stbi_info(Img_Path, &size.x, &size.y, &channels)) {
		throw std::invalid_argument(std::string("Failed to load image: ") + Img_Path);
	}
	return size;

}
