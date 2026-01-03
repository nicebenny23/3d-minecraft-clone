#pragma once
#include "renderer/Vao.h"
#include "renderer/vertexobject.h"
#include "renderer/shader.h"
#include <glm/mat4x4.hpp>
#include "../math/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/texture.h"
#include "renderer/ShaderManager.h"
#include "renderer/renderable.h"
#include "renderer/TextureManager.h"
#include "renderer/Mesh.h"
#include "renderer/RenderProperties.h"
#include "../util/stack.h"
#include "renderer/RenderContext.h"
#include "renderer/vertex.h"
#include "renderer/renderables.h"
#include "../game/ecs/resources.h"
#include "../util/Id.h"
#include "renderer/pass.h"
#include "../util/Name.h"
using namespace buffer_object;
//Fix

namespace renderer {

	enum class indice_mode {
		generate_indices = 0,
		manual_generate = 1,
	};
	struct MeshData {

		size_t components() const {
			return layout.components();
		}
		size_t stride() const {
			return layout.stride();
		}
		size_t length() const {
			return points.length() / components();
		}
		MeshData(mesh_id msh, const vertice::vertex& vertex_layout, indice_mode indices) :mesh(msh), generate_trivial(indices), layout(vertex_layout), points() {
		}
		template<typename ...Args>
		inline void add_point(const Args& ...values) {
			size_t start_len = points.length();

			(push_single(values), ...);

			size_t end_len = points.length();
			size_t floats_pushed = end_len - start_len;

			if (components() != floats_pushed) {
				stn::throw_logic_error("mismatch: number of appended points {} does not match the stated number of components {}", floats_pushed, components());
			}

			if (generate_trivial == indice_mode::generate_indices) {
				indices.push(indices.length());
			}

		}
		void add_indices(std::initializer_list<std::uint32_t> indice_list) {
			indices.append(indice_list);
		}
		void add_index(std::uint32_t index) {
			indices.push(index);
		}
		explicit MeshData() :generate_trivial(indice_mode::generate_indices), points(), indices(), layout() {
		}
		mesh_id mesh;
		indice_mode generate_trivial;
		vertice::vertex layout;
		stn::array<float> points;
		stn::array<std::uint32_t> indices;
	private:
		inline void push_single(const v3::Point3& v) {
			points.push({ float(v.x),float(v.y),float(v.z) });
		}
		inline void push_single(const v2::Vec2& v) {
			points.push({ float(v.x),float(v.y) });
		}
		inline void push_single(float f) {
			points.push(f);
		}
	};
	struct Renderer;
	struct RenderableHandle {
		RenderableHandle() :id(), renderer() {
		}
		RenderableHandle(renderable_id id, Renderer* renderer)
			: id(id), renderer(renderer) {
		}

		void set_material(const std::string& name);

		void set_layout(vertice::vertex layout);

		void fill(MeshData&& new_mesh);

		void set_uniform(const uniforms::uniform& u);

		void render();

		void enable();
		void disable();
		void set_order_key(float key);
		void destroy();
		bool operator==(const RenderableHandle& other) const {
			return other.id == id && other.renderer == renderer;
		}
		MeshData create_mesh(indice_mode auto_ind = indice_mode::manual_generate);
		explicit operator bool() const noexcept {
			return static_cast<bool>(id);
		}
	private:
		Option<renderable_id> id;
		Renderer* renderer;

	};
	struct Renderer : ecs::resource {

		Renderer();
		MaterialManager Modes;
		template <typename... Args>
		void construct_material(const char* name, const char* pass, shader_id id, RenderProperties props, Args&&...args) {
			Modes.construct_material(name, phase_of(pass), id, props, std::forward<Args>(args)...);
		}
		void bind_material(material_id material);

		phase_handle phase_of(std::string name);

		void Bind_Texture(texture_2d_id Handle) {
			context.bind(*Handle);
		}
		void Bind_Texture(texture_array_id Handle) {
			context.bind(*Handle);
		}
		renderer::Context context;
		uniforms::UniformManager uniform_manager;
		template<typename val_type>
		void set_uniform(const char* name, const val_type& val) {

			uniform_manager.set(name, uniforms::uniform_val{ std::in_place_type<val_type>, val });

		}
		void apply_uniform(const uniforms::uniform_val& val, const char* location_in_shader);

		shader& CurrentShader() {
			return context.bound_shader();
		}
		void Clear();

		void Render(Mesh* mesh);

		void setprojmatrix(float newfov, float nearclipplane, float farclipplane);

		RenderableHandle gen_renderable() {
			return RenderableHandle(renderable_list.gen(), this);
		}
		void remove(renderable_id id) {
			renderable& value = renderable_list[id];
			if (value.mesh.bounded()) {
				meshes.remove(value.mesh);
			}
			renderable_list.remove(id);
		}

		void set_material(renderable_id id, std::string material_name) {
			renderable_list.set_material(id, Modes.get_id(material_name));
		}
		void set_uniform(renderable_id id, const uniforms::uniform& value) {
			auto& rend = renderable_list[id];
			for (auto& overide : rend.overides) {
				if (overide.name == value.name) {
					if (value.value.index() != overide.value.index()) {
						throw std::logic_error("uniform may not change type");
					}
					overide = value;
					return;
				}
			}
			renderable_list[id].overides.push(value);
		}

		void set_layout(renderable_id id, vertice::vertex layout) {
			mesh_id  mesh = insert_mesh(id);
			meshes[mesh].Voa.attributes = layout;
		}
		void render(renderable_id id) {
			renderable& value = renderable_list[id];
			if (!value.should_render()) {
				return;
			}
			material_id mat_id = value.material();
			if (mat_id.unbounded()) {
				throw std::logic_error("Material was not set");
			}

			mesh_id mesh_id = value.mesh;

			bind_material(mat_id);
			for (auto& uniform : value.overides) {
				apply_uniform(uniform.value, uniform.name);
			}
			Render(&meshes[mesh_id]);


		}
		ecs::obj entity_of(renderable_id id) {
			return renderable_list[id].object;
		}
		void set_enabled(renderable_id id, bool should_render) {
			renderable_list.set_enabled(id, should_render);
		}
		MeshData create(renderable_id id, indice_mode is_trivial = indice_mode::manual_generate) {
			auto& renderable = renderable_list[id];
			return MeshData(renderable.mesh, meshes[renderable.mesh].Voa.attributes, is_trivial);
		}
		void fill_cmd(MeshData&& mesh) {
			to_fill.push(std::move(mesh));
		}
		void fill_mesh(MeshData& data) {

			data.mesh.assert_bounded("mesh must be bounded to be filled");
			Option<Mesh&> mabye_mesh = meshes.get(data.mesh);
			if (!mabye_mesh) {
				return;
			}
			Mesh& mesh = mabye_mesh.expect("Since we just checked the mesh must now exist");
			if (data.points.length() % mesh.Voa.attributes.components() != 0) {
				throw std::logic_error("Vertex Data is corrupted");
			}
			if (!mesh.BuffersGenerated) {
				throw std::invalid_argument("Cannot Fill a mesh without Generating buffers first");
			}
			context.bind(mesh);
			mesh.Vbo.fillbuffer<float>(data.points);
			mesh.Voa.SetAllAttributes();
			mesh.Ibo.fillbuffer<unsigned int>(data.indices);
			mesh.length = data.indices.length();
		}
		void render_world() {


		}
		void pop() {
			while (!to_fill.empty()) {
				fill_mesh(to_fill.peek());
				to_fill.pop();
			}
		}

		renderables renderable_list;

		float fov;
	private:

		//ensures a mesh exists
		mesh_id insert_mesh(renderable_id id) {

			renderable& value = renderable_list[id];
			if (value.mesh.unbounded()) {
				value.mesh = meshes.create();
			}
			return value.mesh;
		}
		stn::stack<MeshData> to_fill;
		MeshRegistry meshes;

	};

	struct render_pass {
		phase_handle pass;
		stn::array<renderable_id> phase_elements;
		void sort(renderables& world) {
			if (pass->should_sort) {
				phase_elements | stn::sort([world, this](renderable_id p1, renderable_id p2) {
					return world[p1].get_order() > world[p2].get_order(); });
			}
		}
		std::string_view name() const {
			return pass->name;
		}

		render_pass(phase_handle& pass) :pass(pass), phase_elements() {

		}
	};
	struct render_all :ecs::System {
		void run(ecs::Ecs& world) {
			
			Renderer& ren = world.get_resource<Renderer>().unwrap();
			std::unordered_map<phase_handle,render_pass> pass_map;

			for (renderable& renderable : ren.renderable_list) {
				if (renderable.should_render()) {
					phase_handle pass = ren.Modes[renderable.material()].pass;
					pass_map.try_emplace(pass, render_pass(pass));
					pass_map.at(pass).phase_elements.emplace(renderable.id);
				}
				
			}
			stn::array<render_pass> passes;
			for (auto& [key, value] : pass_map) {
				passes.push(std::move(value));
			}
			pass_map.clear();
			for (render_pass& pass : passes) {
				pass.sort(ren.renderable_list);
			}
			passes | stn::sort([](const render_pass& a, const render_pass& b) {
				return a.pass->priority <b.pass->priority;
			});
			for (render_pass& pass: passes) {
				for(renderable_id id: pass.phase_elements){
					ren.render(id);
				}
			}
		}
	};

}