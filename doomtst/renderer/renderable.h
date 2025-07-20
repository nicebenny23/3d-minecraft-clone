#include "meshStorage.h"
#include "Uniform.h"
#pragma once
struct renderable {
	Meshes::meshId mesh;

	stn::array<uniforms::uniform> overides;
};