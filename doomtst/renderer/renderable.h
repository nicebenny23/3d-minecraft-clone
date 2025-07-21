#include "meshStorage.h"
#include "Uniform.h"
#pragma once
struct renderable {
	Meshes::meshId mesh;
	size_t material;
	template <typename T>
	void set(const char* name, T& value) {
		
		for (auto& val:overides)
		{
			if (val.name==name)
			{
				val.value = value;
				return;
			}
			
		}
		overides.push(uniforms::uniform(name,value));

	}
	stn::array<uniforms::uniform> overides;
	~renderable() {
		overides.destroy();
	}

};
