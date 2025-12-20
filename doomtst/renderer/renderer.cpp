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






	Renderer::Renderer() :Shaders(), Textures(), uniform_manager() {
		Modes = MaterialManager(&Shaders, &uniform_manager);
		fov = 90;
		setprojmatrix(90, .21f, 100);
		Shaders.Compile("UiShader", "shaders\\uivertex.vs", "shaders\\uifragment.vs");
		construct_material("Ui", "UiShader", RenderProperties(false, false, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			uniforms::uparam("aspect_ratio", "aspectratio")
		);
		Shaders.Compile("ModelShader", "shaders\\modelvertex.vs", "shaders\\modelfragment.vs");
		construct_material("Model", "ModelShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA),
			uniforms::uparam("aspect_ratio", "aspectratio"),
			uniforms::uparam("proj_matrix", "projection"),
			uniforms::uparam("view_matrix", "view")

		);
		Shaders.Compile("ParticleShader", "shaders\\particlevertex.vs", "shaders\\particlefragment.vs");
		construct_material("Particle", "ParticleShader", RenderProperties(true, true, false, true, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		set_uniform("aspect_ratio", CtxName::ctx.Window->AspectRatio());
		meshes = MeshRegistry(&context);
	}

	void Renderer::setprojmatrix(float newfov, float nearclipplane, float farclipplane) {
		set_uniform("proj_matrix", glm::perspective(glm::radians(newfov), float(4 / 3.f), nearclipplane, farclipplane));
	}

	void Renderer::bind_material(material_id material) {
		const Material& mat = Modes[material];
		context.bind(Shaders.shader_of(mat.shader));
		for (const auto& elem : mat.handles) {
			apply_uniform(uniform_manager.get(elem), elem.name);
		}
		context.bind_properties(mat.prop);
	}

	void Renderer::apply_uniform(const uniforms::uniform_val& val, const char* location_in_shader) {
		size_t ind = val.index();
		switch (val.index()) {
		case uniforms::uform_int:
		context.bound_shader().seti(std::get<int>(val), location_in_shader);
		break;
		case uniforms::uform_float:
		context.bound_shader().setf(std::get<float>(val), location_in_shader);
		break;
		case uniforms::uform_gluint:
		context.bound_shader().setuint(std::get<unsigned int>(val), location_in_shader);
		break;
		case uniforms::uform_vec2:
		context.bound_shader().SetVector2f(std::get<v2::Vec2>(val), location_in_shader);
		break;
		case uniforms::uform_vec3:
		context.bound_shader().SetVector3f(std::get<v3::Vec3>(val), location_in_shader);
		break;
		case uniforms::uform_vec4:
		context.bound_shader().SetVector4f(std::get<glm::vec4>(val), location_in_shader);
		break;
		case uniforms::uform_mat3:
		context.bound_shader().setMat3(std::get<glm::mat3>(val), location_in_shader);
		break;
		case uniforms::uform_mat4:
		context.bound_shader().setMat4(std::get<glm::mat4>(val), location_in_shader);
		break;
		case uniforms::uform_tex:
		Textures.get_texture(std::get<texture_id>(val)).apply();
		break;
		default:
		throw std::logic_error("Invalid uniform paramater");
		break;
		}
	}






	void Renderer::Clear() {

		set_uniform("aspect_ratio", CtxName::ctx.Window->AspectRatio());
		glClearColor(0, 0, 0, 0.0f);
		glDepthMask(GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthMask(GL_FALSE);
	}

	void Renderer::SetUniform(const std::string& name, float value) {
		CurrentShader().setf(value, name.c_str());

	}
	void Renderer::SetUniform(const std::string& name, const glm::vec2& vec) {
		CurrentShader().SetVector2f(vec, name.c_str());
	}

	void Renderer::SetUniform(const std::string& name, const glm::vec3& vec) {
		CurrentShader().SetVector3f(vec, name.c_str());

	}

	void Renderer::SetUniform(const std::string& name, const glm::vec4& vec) {

		CurrentShader().SetVector4f(vec, name.c_str());
	}

	void Renderer::SetUniformMat4(const std::string& name, const glm::mat4& mat) {
		CurrentShader().setMat4(mat, name.c_str());
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
