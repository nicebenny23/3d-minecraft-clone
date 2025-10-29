#pragma once
#include "Vao.h"
#include "vertexobject.h"
#include "shader.h"
#include <glm/mat4x4.hpp>
#include "../util/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "ShaderManager.h"
#include "renderable.h"
#include "VertexObjectManager.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "RenderProperties.h"
#include "../util/stack.h"
#include "RenderContext.h"
#include "vertex.h"
//#inc
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
	template<typename T> struct dependent_false : std::false_type {};
	struct MeshData {
		Ids::Id mesh;
		

		indice_mode generate_trivial;
		size_t length() {
			
			return pointlist.length() * sizeof(float) / stride;

		}
		MeshData(Ids::Id msh,const vertice::vertex& vertex_layout, indice_mode indices) :mesh(msh),generate_trivial(indices),layout(vertex_layout), pointlist()
		{
			stride = layout.stride();

		}

		~MeshData() {

		}
		template<typename ...Args>
		inline void add_point(const Args& ...values)
		{
			size_t pushed_floats = 0;
			(void)std::initializer_list<int>{
				(pushed_floats += push_single(values), 0)...
			};

			if (stride != pushed_floats * sizeof(float)) {
				throw std::logic_error("Point size mismatch: does not match stride");
			}

			if (generate_trivial==indice_mode::generate_indices) {
				indicelist.push(indicelist.length());
			}

		}
		template<typename T>
		inline size_t push_single(const T& value)
		{
			
				if constexpr (std::is_same_v<T, v3::Vec3>) {
					pointlist.push({ value.x, value.y, value.z });
					return 3;
				}
				else if constexpr (std::is_same_v<T, v2::Vec2>) {
					pointlist.push({value.x, value.y});
					return 2;
				}
				else if constexpr (std::is_convertible_v<T, float>) {
					pointlist.push(static_cast<float>(value));
					return 1;
				}
				else {
					static_assert(dependent_false<T>::value, "Invalid type passed to add_point");
				}
			
		}
		void add_index(size_t index) {
			indicelist.push(index);
		}
		explicit MeshData() :generate_trivial(indice_mode::generate_indices), pointlist(), indicelist(), layout(),stride() {}

		size_t stride;
		vertice::vertex layout;
		friend Renderer;
		stn::array<float> pointlist;
		stn::array<unsigned int> indicelist;
	};
	struct RenderableHandle {
		Ids::Id id;
		Renderer* renderer;
		RenderableHandle():id(),renderer() {


		}
		RenderableHandle(Ids::Id id, Renderer* renderer)
			: id(id), renderer(renderer) {
		}
		
		void set_material(const std::string& name);
		
		void set_layout(vertice::vertex layout);
		
		void fill(MeshData&& new_mesh);
		
		void set_uniform(const uniforms::uniform& u);
		
		void render();
		
		void destroy();
		MeshData create_mesh(indice_mode auto_ind = indice_mode::manual_generate);
		explicit operator bool() const noexcept {
			return id.valid();
		}
	};
	struct Renderer {

		
		Shaders::ShaderManager Shaders;
		VObjMan::VObjManager Binders;
		TextureManager::TextureManager Textures;

		MaterialManager Modes;
		template <typename... Args>
		void Construct(const char* name, const char* shade, RenderProperties props, Args&&...args) {
			Modes.Construct(name, shade, props, std::forward<Args>(args)...);
		}
		void bind_material(Ids::Id material);
		void SetType(std::string Name);


		void Bind_Texture(Ids::Id Handle) {
			context.Bind( Textures.get_texture(Handle));

		}
		Renderer();
		void InitilizeBaseMaterials();	
		RenderContext::Context context;
	
		
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

		
		shader* CurrentShader() {
			return context.Get_BoundShader();
		}
		void Clear();

		void Gen(Mesh* mesh);
		void Destroy(Mesh* mesh);

		void FillVertexBuffer(Mesh* mesh, stn::array<float>& pointlist);
		void Fill(Mesh* mesh, stn::array<float>& pointlist);
		void Fill(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist);
		void Render(Mesh* mesh);
		void Render(Mesh* mesh, stn::array<float>& pointlist);
		void Render(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist);
		
		void setprojmatrix(float newfov, float nearclipplane, float farclipplane);
	
		RenderableHandle gen_renderable() {
			if (free_ids.empty())
			{
				free_ids.push(Ids::Id(renderables.length()));
			}
			
			Ids::Id id = free_ids.pop();
			renderables.push(id.id, renderable());
			return RenderableHandle(Ids::Id(id),this);
		}
		
		void set_material(Ids::Id renderable_id,std::string material_name) {

			renderable& value = renderables[renderable_id.id];
			value.material = Modes.get_id(material_name);
		}
		void set_uniform(Ids::Id renderable_id, const uniforms::uniform& value) {
			auto& rend = renderables[renderable_id.id];
			for (auto i = 0; i < rend.overides.length(); i++)
			{
				if (rend.overides[i].name==value.name)
				{
					if (value.value.index() != rend.overides[i].value.index()) {
						throw std::logic_error("uniform may not change type");
					}
					rend.overides[i] = value;
					return;
				}
			}
			renderables[renderable_id.id].overides.push(value);
		}
		
		
	
		void set_layout(Ids::Id renderable_id, vertice::vertex layout) {
			Ids::Id mesh_id = ensure_mesh(renderable_id);
			meshes[mesh_id].Voa.attributes = layout;
		}
		void render(Ids::Id renderable_id) {

			renderable& value = renderables[renderable_id.id];
			Ids::Id mat_id = value.material;
			if (!mat_id)
			{
				throw std::logic_error("Material was not set");
			}
			Ids::Id mesh_id = value.mesh;
			bind_material(mat_id);
			for (auto& uniform : value.overides)
			{
				apply_uniform(uniform.value, uniform.name);
			}
			Render(&meshes[mesh_id]);
		}
		void remove(Ids::Id renderable_id) {
			renderable* value = &renderables[renderable_id.id];
			if (!value)
			{
				throw std::logic_error("unable to delete invalid mesh");
			}
			if (value->mesh.valid())
			{
				meshes.remove(value->mesh);
			}
			renderables.erase_key(renderable_id.id);
			free_ids.push(renderable_id);
		}
		void fill_cmd(Ids::Id renderable_id, MeshData&& mesh) {
			Ids::Id mesh_id = ensure_mesh(renderable_id);
			to_fill.push(std::move(mesh));
		}
		void fill_mesh(MeshData& data) {
			//must exist as a precurus as it is created through fillcmd;
			Ids::Id mesh_id = data.mesh;
			Fill(&meshes[mesh_id], data.pointlist, data.indicelist);
		}
		void pop() {
			while (!to_fill.empty())
			{
				fill_mesh(to_fill.peek());
				to_fill.pop();
			}
		}
		
		MeshData create(Ids::Id renderable_id, indice_mode is_trivial= indice_mode::manual_generate) {
			auto& renderable = renderables[renderable_id.id];
			return MeshData(renderable.mesh,meshes[renderable.mesh].Voa.attributes, is_trivial);
		}
		float fov;
		private:
			
		
		Ids::Id ensure_mesh(Ids::Id renderable_id) {

			renderable& value = renderables[renderable_id.id];
			if (!value.mesh.valid())
			{
				value.mesh = meshes.create();
			}
			return value.mesh;
		}
		stn::stack<MeshData> to_fill;
		Meshes::MeshRegistry meshes;
		Sparse::KeySet<renderable> renderables;
		stn::array<Ids::Id> free_ids;
		

		};

		inline void Renderer::Gen(Mesh* mesh)
		{
			Binders.Create(&mesh->Ibo);
			Binders.Create(&mesh->Voa);
			Binders.Create(&mesh->Vbo);
			mesh->BuffersGenerated = true;
		}
	
		
}

