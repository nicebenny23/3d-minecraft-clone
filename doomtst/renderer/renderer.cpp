#include "renderer.h"
#include "../game/Core.h"
#include "../game/Settings.h"
#include "../game/GameContext.h"
#include "decal.h"
namespace renderer {


	void Renderer::Render(Mesh* mesh) {

		if (!mesh->filled()) {
			return;
		}

		context.bind(*mesh);

		mesh->Voa.SetAllAttributes();
		if (settings::Gamesettings.viewmode) {
			glDrawElements(GL_LINES, mesh->length, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawElements(GL_TRIANGLES, mesh->length, GL_UNSIGNED_INT, 0);
		}
	}






	Renderer::Renderer():uniform_manager() {
		Modes = MaterialManager(&uniform_manager);
		fov = 90;
		setprojmatrix(90, .21f, 100);
		renderer::shader_id ui_shader= CtxName::ctx.Ecs->load_asset_emplaced<shader_load>("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs").unwrap();
		CtxName::ctx.Ecs->load_asset(render_phase(12, true, "ui_phase"));
		CtxName::ctx.Ecs->load_asset(render_phase(0, false, "solid_phase"));
		CtxName::ctx.Ecs->load_asset(render_phase(1, true, "transparent_phase"));

		construct_material("Ui","ui_phase", ui_shader, RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			{ uniforms::uparam("aspect_ratio", "aspectratio") }
		);
		renderer::shader_id model_shader = CtxName::ctx.Ecs->load_asset_emplaced<shader_load>("ModelShader", "shaders\\modelvertex.vs", "shaders\\modelfragment.vs").unwrap();
		construct_material("Model","solid_phase", model_shader, RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			uniforms::uparam("aspect_ratio", "aspectratio"),
			uniforms::uparam("proj_matrix", "projection"),
			uniforms::uparam("view_matrix", "view")

		);
		renderer::shader_id particle_shader = CtxName::ctx.Ecs->load_asset_emplaced<shader_load>("ModelShader", "shaders\\modelvertex.vs", "shaders\\modelfragment.vs").unwrap();

		CtxName::ctx.Ecs->load_asset_emplaced<shader_load>("ParticleShader", "shaders\\particlevertex.vs", "shaders\\particlefragment.vs");
		construct_material("Particle", "solid_phase", particle_shader, RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		set_uniform("aspect_ratio", CtxName::ctx.Window->AspectRatio());
		meshes = MeshRegistry(&context);
	}

	void Renderer::setprojmatrix(float newfov, float nearclipplane, float farclipplane) {
		set_uniform("proj_matrix", glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane));
	}

	void Renderer::bind_material(material_id material) {
		Material& mat = Modes[material];
		context.bind(*mat.shader);
		for (const auto& elem : mat.handles) {
			apply_uniform(uniform_manager.get(elem), elem.name);
		}
		context.bind_properties(mat.prop);
	}

	phase_handle Renderer::phase_of(std::string name) {
		return CtxName::ctx.Ecs->from_name<render_phase>(name).expect("asset handle should exist");
	}

	void Renderer::apply_uniform(const uniforms::uniform_val& val, const char* location_in_shader) {
		context.set_uniform(uniforms::uniform(val, location_in_shader));
	}






	void Renderer::Clear() {

		set_uniform("aspect_ratio", CtxName::ctx.Window->AspectRatio());
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
		renderer->fill_cmd(std::move(new_mesh));
	}

	void RenderableHandle::disable() {
		renderer->set_enabled(id.unwrap(), false);
	}

	void RenderableHandle::set_order_key(float key) {
		stn::Option<order_key&> order= renderer->entity_of(id.unwrap()).get_component_opt<order_key>();
		if (!order) {
			renderer->entity_of(id.unwrap()).add_component<order_key>(key);
		}
		else {
			order.unwrap().order = key;
		}
	}

	void RenderableHandle::enable() {

		renderer->set_enabled(id.unwrap(), true);
	}
	void RenderableHandle::set_uniform(const uniforms::uniform& u) {
		renderer->set_uniform(id.unwrap(), u);
	}

	void RenderableHandle::render() {
		renderer->render(id.unwrap());
	}

	void RenderableHandle::destroy() {

		renderer->remove(id.unwrap());
		renderer = nullptr;
		id = None;
	}

	MeshData RenderableHandle::create_mesh(indice_mode auto_ind) {
		return renderer->create(id.unwrap(), auto_ind);
	}


}