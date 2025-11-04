#pragma once
#include "renderer/Vao.h"
#include "renderer/vertexobject.h"
#include "renderer/shader.h"
#include <glm/mat4x4.hpp>
#include "../util/vector3.h"
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
#include "../util/Id.h"
using namespace buffer_object;
//Fix

namespace renderer {
	struct Renderer;
	
	inline array<unsigned int> trivial_buffer(vertice::vertex& layout, stn::array<float>& points) {
		array<unsigned int> trivial;
		size_t vertices = points.length() / layout.stride();
		for (size_t i = 0; i < vertices; i++)
		{
			trivial.push(i);
		}
		return trivial;
	}
	enum class indice_mode {
		generate_indices = 0,
		manual_generate = 1,
	};
	struct MeshData {
		
		size_t components() const{
			return layout.components();
		}
		size_t stride() const {
			return layout.stride();
		}
		size_t length() const {
			return pointlist.length()/ components();
		}
		MeshData(mesh_id msh,const vertice::vertex& vertex_layout, indice_mode indices) :mesh(msh),generate_trivial(indices),layout(vertex_layout), pointlist(){}
		template<typename ...Args>
		inline void add_point(const Args& ...values)
		{
			size_t start_len = pointlist.length();

			(push_single(values), ...);

			size_t end_len = pointlist.length();
			size_t floats_pushed = end_len - start_len;

			if (components() != floats_pushed) {
				stn::throw_logic_error("mismatch: number of appended points {} does not match the stated number of components {}",floats_pushed, components());
			}

			if (generate_trivial==indice_mode::generate_indices) {
				indicelist.push(indicelist.length());
			}

		}
		
		void add_index(size_t index) {
			indicelist.push(index);
		}
		explicit MeshData() :generate_trivial(indice_mode::generate_indices), pointlist(), indicelist(), layout(){}
		mesh_id mesh;
		indice_mode generate_trivial;
		vertice::vertex layout;
		friend Renderer;
		stn::array<float> pointlist;
		stn::array<unsigned int> indicelist;
	private:
		inline void push_single(const v3::Vec3& v) {
			pointlist.push({ float(v.x),float(v.y),float(v.z) });
		}
		inline void push_single(const v2::Vec2& v) {
			pointlist.push({ float(v.x),float(v.y) });
		}
		inline void push_single(float f) {
			pointlist.push(f);
		}
	};
	struct RenderableHandle {
		Option<renderable_id> id;
		Renderer* renderer;
		RenderableHandle():id(),renderer() {


		}
		RenderableHandle(renderable_id id, Renderer* renderer)
			: id(id), renderer(renderer) {
		}
		
		void set_material(const std::string& name);
		
		void set_layout(vertice::vertex layout);
		
		void fill(MeshData&& new_mesh);
		
		void set_uniform(const uniforms::uniform& u);
		
		void render();
		void disable();

		void enable();
		void destroy();
		bool operator==(const RenderableHandle& other) const{
			return other.id == id && other.renderer == renderer;
		}
		MeshData create_mesh(indice_mode auto_ind = indice_mode::manual_generate);
		explicit operator bool() const noexcept {
			return static_cast<bool>(id);
		}
	};
	struct Renderer {

		Renderer();
		void InitilizeBaseMaterials();

		renderer::Shaders Shaders;
		TextureManager Textures;

		MaterialManager Modes;
		template <typename... Args>
		void construct_material(const char* name, const char* shade, RenderProperties props, Args&&...args) {
			Modes.construct_material(name, shade, props, std::forward<Args>(args)...);
		}
		void bind_material(material_id material);



		void Bind_Texture(texture_id Handle) {
			context.bind(Textures.get_texture(Handle));

		}
		renderer::Context context;
		uniforms::UniformManager uniform_manager;
		template<typename val_type>
		void set_uniform(const char* name, const val_type& val) {

			uniform_manager.set(name, uniforms::uniform_val{ std::in_place_type<val_type>, val });

		}
		void apply_uniform(const uniforms::uniform_val& val, const char* location_in_shader);
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, const glm::vec2& vec);
		void SetUniform(const std::string& name, const glm::vec3& vec);
		void SetUniform(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4(const std::string& name, const glm::mat4& mat);
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
			if (value.mesh.bounded())
			{
				meshes.remove(value.mesh);
			}
			renderable_list.remove(id);
		}

		void set_material(renderable_id id,std::string material_name) {
			renderable_list.set_material(id, Modes.get_id(material_name));
		}
		void set_uniform(renderable_id id, const uniforms::uniform& value) {
			auto& rend = renderable_list[id];
			for (auto& overide:rend.overides)
			{
				if (overide.name==value.name)
				{
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
			mesh_id  mesh= insert_mesh(id);
			meshes[mesh].Voa.attributes = layout;
		}
		void render(renderable_id id) {
			renderable& value = renderable_list[id];
			if (!value.should_render)
			{
				return;
			}
			material_id mat_id = value.material;
			if (mat_id.unbounded())
			{
				throw std::logic_error("Material was not set");
			}

			mesh_id mesh_id = value.mesh;

			bind_material(mat_id);
			for (auto& uniform : value.overides)
			{
				apply_uniform(uniform.value, uniform.name);
			}
			Render(&meshes[mesh_id]);

			
		}
	
		void set_enabled(renderable_id id, bool should_render) {
			renderable_list.set_enabled(id,should_render);
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
			Mesh& mesh = meshes[data.mesh];
			if (data.pointlist.length() % mesh.Voa.attributes.components() != 0)
			{
				throw std::logic_error("Vertex Data is corrupted");
			}
			if (!mesh.BuffersGenerated)
			{
				throw std::invalid_argument("Cannot Fill a mesh without Generating buffers first");
			}
			context.bind(mesh);
			mesh.Vbo.fillbuffer<float>(data.pointlist);
			mesh.Voa.SetAllAttributes();
			mesh.Ibo.fillbuffer<unsigned int>(data.indicelist);
			mesh.length = data.indicelist.length();
		}

		void pop() {
			while (!to_fill.empty())
			{
				fill_mesh(to_fill.peek());
				to_fill.pop();
			}
		}
		
	
		float fov;
		private:
			
		//ensures a mesh exists
		mesh_id insert_mesh(renderable_id id) {

			renderable& value = renderable_list[id];
			if (value.mesh.unbounded())
			{
				value.mesh = meshes.create();
			}
			return value.mesh;
		}
		stn::stack<MeshData> to_fill;
		MeshRegistry meshes;
		renderables renderable_list;

	};

	
		
}

