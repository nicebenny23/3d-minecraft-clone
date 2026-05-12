#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"
namespace renderer {
	void RenderableHandle::give_owned_mesh() {
		id.get_component<mesh_component>().msh = renderer().make_mesh();

	}
	MeshBuilder RenderableHandle::insert_builder_for(const renderer::vertex& vertex, indice_mode auto_ind)
	{
		give_owned_mesh();
		return MeshBuilder(id.get<mesh_component>().msh.unwrap(),vertex,auto_ind);
	}
	void RenderableHandle::destroy() {
		world().write_command(remove_render_object(id.object()));
	}
}