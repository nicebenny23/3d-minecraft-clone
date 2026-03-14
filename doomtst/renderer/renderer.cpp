#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"

#include "decal.h"
namespace renderer {
	MeshData RenderableHandle::create_mesh(vertice::vertex& vertex,indice_mode auto_ind)
	{
		return MeshData(mesh(),vertex, auto_ind);
	}
	mesh_id RenderableHandle::mesh() {
		
		return renderer().insert_mesh(id.unwrap());
	}
	void RenderableHandle::destroy() {
		if (id) {
			renderer().remove(id.unwrap());
			id = None;

		}
	}
}