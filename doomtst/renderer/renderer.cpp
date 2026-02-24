#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"

#include "decal.h"
namespace renderer {
	void RenderableHandle::set_layout(vertice::vertex layout) {
		renderer().set_layout(id.unwrap(), layout);
	}
	MeshData RenderableHandle::create_mesh(indice_mode auto_ind) {
		return renderer().create(id.unwrap(), auto_ind);
	}
	void RenderableHandle::destroy() {
		if (id) {
			renderer().remove(id.unwrap());
			id = None;

		}
	}
}