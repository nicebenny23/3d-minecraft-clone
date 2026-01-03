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
			if (should_render) {
				renderable_list[id.id].enable();
			}
			else {
				renderable_list[id.id].disable();
			}
		}
		void set_mesh(renderable_id id, mesh_id msh) {
			renderable_list[id.id].mesh = msh;
		}
		void set_material(renderable_id id, material_id material) {
			renderable_list[id.id].set_material(material);
		}
		size_t total_renderables() const {
			return renderable_list.length();
		}
		renderable_id gen() {
			if (free_ids.empty()) {
				free_ids.push(renderable_id(static_cast<uint32_t>(total_renderables())));
			}
			renderable_id id = free_ids.pop();
			renderable_list.push(id.id, renderable(id));
			return id;
		}		
		using iterator = typename Sparse::KeySet<renderable>::iterator;
		using const_iterator = typename Sparse::KeySet<renderable>::const_iterator;
		iterator begin() {
			return renderable_list.begin();
		}
		iterator end() {
			return renderable_list.end();
		}
		const_iterator begin() const {
			return renderable_list.begin();
		}
		const_iterator end()   const {
			return renderable_list.end();
		}
	};
}