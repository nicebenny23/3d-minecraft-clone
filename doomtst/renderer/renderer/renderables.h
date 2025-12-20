#include "renderable.h"
#pragma once
namespace renderer {

	struct renderables {

		Sparse::KeySet<renderable> renderable_list;
		stn::array<renderable_id> free_ids;
		const renderable& operator[](renderable_id id) const {
			return renderable_list[id.id];
		}
		renderable& operator[](renderable_id id) {
			return renderable_list[id.id];
		}
		void remove(renderable_id id) {
			renderable value = renderable_list[id.id];

			renderable_list.erase_key(id.id);
			free_ids.push(id);
		}
		void set_enabled(renderable_id id, bool should_render) {
			renderable_list[id.id].should_render = should_render;
		}
		void set_mesh(renderable_id id, mesh_id msh) {
			renderable_list[id.id].mesh = msh;
		}
		void set_material(renderable_id id, material_id material) {
			renderable_list[id.id].material = material;
		}
		size_t total_renderables() const {
			return renderable_list.length();
		}
		renderable_id gen() {
			if (free_ids.empty()) {
				//1
				free_ids.push(renderable_id(static_cast<uint32_t>(total_renderables())));
			}
			renderable_id id = free_ids.pop();
			renderable_list.push(id.id, renderable());
			return id;
		}
	};
}