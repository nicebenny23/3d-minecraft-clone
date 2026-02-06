#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"
#include "../game/GameContext.h"
#include "decal.h"
namespace renderer {


	void Renderer::Render(Mesh& mesh) {

		if (!mesh.filled()) {
			return;
		}

		context.bind(mesh);

		mesh.Voa.SetAllAttributes();
		if (settings::Gamesettings.viewmode) {
			glDrawElements(GL_LINES, mesh.length, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawElements(GL_TRIANGLES, mesh.length, GL_UNSIGNED_INT, 0);
		}
	}






	void Renderer::setprojmatrix(float newfov, float nearclipplane, float farclipplane) {
		set_uniform("proj_matrix", glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane));
	}

	void Renderer::set_material(renderable id, std::string name) {
	id.set_material(world.from_name<Material>(name).expect("material should exist"));
	}

	void Renderer::bind_material(material_handle material) {
		Material& mat = *material;
		context.bind(*mat.shader);
		for (const auto& elem : mat.handles) {
			apply_uniform(uniform_manager.get(elem), elem.name);
		}
		context.bind_properties(mat.prop);
	}
	void Renderer::apply_uniform(const renderer::uniform_val& val, const std::string& location_in_shader) {
		context.set_uniform(renderer::uniform(val, location_in_shader.c_str()));
	}






	void Renderer::Clear() {

		set_uniform("aspect_ratio", world.get_resource<window::Window>().unwrap().AspectRatio());
		glClearColor(0, 0, 0, 0.0f);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_FALSE);
	}


	void RenderableHandle::set_material(const std::string& name) {
		renderer->set_material(id.unwrap(), name);
	}

	void RenderableHandle::set_layout(vertice::vertex layout) {
		renderer->set_layout(id.unwrap(), layout);
	}

	void RenderableHandle::fill(MeshData&& new_mesh) {
		CtxName::ctx.Ecs->write_command(std::move(new_mesh));
	}

	void RenderableHandle::disable() {
		renderer->set_enabled(id.unwrap(), false);
	}

	void RenderableHandle::set_order_key(float key) {
		id.unwrap().set_order(key);
	}

	void RenderableHandle::enable() {

		renderer->set_enabled(id.unwrap(), true);
	}
	void RenderableHandle::set_uniform(const renderer::uniform& u) {
		renderer->set_uniform(id.unwrap(), u);
	}

	void RenderableHandle::render() {
		renderer->render(id.unwrap());
	}

	void RenderableHandle::destroy() {
		if (renderer) {

			renderer->remove(id.unwrap());
			renderer = nullptr;
			id = None;
		}
	}

	MeshData RenderableHandle::create_mesh(indice_mode auto_ind) {
		return renderer->create(id.unwrap(), auto_ind);
	}


}