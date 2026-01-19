#pragma once
#include "renderer/Vao.h"
#include "renderer/vertexobject.h"
#include "renderer/shader.h"
#include <glm/mat4x4.hpp>
#include "../math/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "renderer/texture.h"
#include "renderer/renderable.h"
#include "renderer/Mesh.h"
#include "renderer/RenderProperties.h"
#include "../util/stack.h"
#include "renderer/RenderContext.h"
#include "renderer/vertex.h"
#include "../game/ecs/resources.h"
#include "../util/Id.h"
#include "../game/ecs/query.h"
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
		template<typename Range> requires convertible_range<Range, std::uint32_t>
		void add_indices(const Range& indice_list) {
			indices.append(indice_list);
		}
		void add_index(std::uint32_t index) {
			indices.push(index);
		}
		template<typename Range> requires convertible_range<Range,std::uint32_t>
		void add_indices_offset_from(const Range& indice_list, std::uint32_t offset) {
			for (auto i : indice_list) {
				indices.push(i + offset);
			}
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
		RenderableHandle(renderable id, Renderer* renderer)
			: id(id), renderer(renderer) {
		}

		void set_material(const std::string& name);

		void set_layout(vertice::vertex layout);

		void fill(MeshData&& new_mesh);

		void set_uniform(const renderer::uniform& u);

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
		Option<renderable> id;
		Renderer* renderer;

	};

	struct remove_render_object {
		ecs::obj object;
	};
	struct Renderer : ecs::resource {

		Renderer();

		void bind_material(material_handle material);

		void Bind_Texture(texture_2d_id Handle) {
			context.bind(*Handle);
		}
		void Bind_Texture(texture_array_id Handle) {
			context.bind(*Handle);
		}
		renderer::Context context;
		renderer::UniformManager uniform_manager;
		template<typename val_type>
		void set_uniform(const char* name, const val_type& val) {

			uniform_manager.set(name, renderer::uniform_val{ std::in_place_type<val_type>, val });

		}
		void apply_uniform(const renderer::uniform_val& val, const std::string& location_in_shader);

		void Clear();

		void Render(Mesh& mesh);

		void setprojmatrix(float newfov, float nearclipplane, float farclipplane);
		void set_material(renderable id, std::string name);

		RenderableHandle gen_renderable() {
			return RenderableHandle(renderable(), this);
		}
		void remove(renderable ren) {
			ren.object.world().write_command(remove_render_object(ren.object));
		}

		void set_uniform(renderable rend, const renderer::uniform& value) {
			rend.set(value);
		}

		void set_layout(renderer::renderable id, vertice::vertex layout) {
			mesh_id  mesh = insert_mesh(id);
			meshes[mesh].Voa.attributes = layout;
		}
		void render(renderer::renderable ren) {
			if (!ren.should_render()) {
				return;
			}
			material_handle mat_id = ren.material();
			mesh_id mesh_id = ren.get_mesh();

			bind_material(mat_id);
			for (auto& uniform : ren.view_overides()) {
				apply_uniform(uniform.value, uniform.name);
			}
			Render(meshes[mesh_id]);


		}

		void set_enabled(renderable ren, bool should_render) {
			if (should_render) {
				ren.enable();
			}
			else {
				ren.disable();
			}
		}
		MeshData create(renderable id, indice_mode is_trivial = indice_mode::manual_generate) {

			return MeshData(id.get_mesh(), meshes[id.get_mesh()].Voa.attributes, is_trivial);
		}
		void fill_mesh(MeshData& data) {
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

		MeshRegistry meshes;
		float fov;
	private:

		//ensures a mesh exists
		mesh_id insert_mesh(renderer::renderable id) {

			if (!id.has_component<mesh_component>() || id.get_component<mesh_component>().msh.is_none()) {
				id.set_mesh(meshes.create());
	
			}
			return id.get_mesh();
		}

	};

	struct render_pass {
		phase_handle pass;
		stn::array<ecs::obj> phase_elements;
		void sort() {
			if (pass->should_sort) {
				phase_elements | stn::sort([this](ecs::obj p1) {return p1.get_component<order_key>().order; });
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

			for (MeshData& mesh : world.read_commands<MeshData>()) {
				ren.fill_mesh(mesh);
			}

			std::unordered_map<phase_handle, render_pass> pass_map;

			ecs::View<material_component, order_key, is_enabled> renderable_iter(world);
			for (auto&& [mat, order, should_render] : renderable_iter) {
				if (should_render.enabled) {
					phase_handle pass = mat.mat_id->pass;
					pass_map.try_emplace(pass, render_pass(pass));
					pass_map.at(pass).phase_elements.emplace(mat.owner());
				}

			}
			stn::array<render_pass> passes;
			for (auto& [key, value] : pass_map) {
				passes.push(std::move(value));
			}
			pass_map.clear();
			for (render_pass& pass : passes) {
				pass.sort();
			}
			passes | stn::sort([](const render_pass& a) {return a.pass->priority;});
			for (render_pass& pass : passes) {
				for (ecs::obj id : pass.phase_elements) {
					ren.render(renderable(id));
				}
			}
			for (remove_render_object to_remove:world.read_commands<remove_render_object>()) {
				mesh_component& mesh_comp = to_remove.object.get_component<mesh_component>();
				if (mesh_comp.msh) {
					ren.meshes.remove(mesh_comp.msh.unwrap());
				}
				mesh_comp.owner().destroy();
			}
		}
	};

}