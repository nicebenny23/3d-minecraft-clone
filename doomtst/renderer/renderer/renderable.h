#include "meshStorage.h"
#include "Uniform.h"
#include "../util/Id.h"
#include "RenderProperties.h"
#pragma once
namespace renderer {
	struct renderable_tag;
	using renderable_id = stn::typed_id<renderable_tag>;
	struct renderable {
		renderer::mesh_id mesh;
		material_id material;
		bool should_render;
		template <typename T>
		void set(const char* shader_alias, T& value) {

			for (auto& val : overides)
			{
				if (val.name == shader_alias)
				{
					val.id = value;
					return;
				}

			}
			overides.push(uniforms::uniform(shader_alias, value));

		}
		renderable() :should_render(true) {


		}
		stn::array<uniforms::uniform> overides;
		~renderable() {
		}
		void set_material(material_id mat_id) {
			material = mat_id;
		}
		void set_mesh(renderer::mesh_id msh_id) {
			mesh = msh_id;
		}
	};
}