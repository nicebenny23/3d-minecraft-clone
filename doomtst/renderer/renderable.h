#include "meshStorage.h"
#include "Uniform.h"
#pragma once
struct renderable {
	Ids::Id mesh;
	Ids::Id material;
	template <typename T>
	void set(const char* shader_alias, T& value) {
		
		for (auto& val:overides)
		{
			if (val.name==shader_alias)
			{
				val.id = value;
				return;
			}
			
		}
		overides.push(uniforms::uniform(shader_alias,value));

	}
	renderable() {


	}
	stn::array<uniforms::uniform> overides;
	~renderable() {
	}
	void set_material(Ids::Id mat_id) {
		material = mat_id;
	}
	void set_mesh(Ids::Id msh_id) {
		mesh = msh_id;
	}
};
