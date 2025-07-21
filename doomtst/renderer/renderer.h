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
	struct meshId {
		size_t id;
		Renderer* ren;

	};
	inline array<unsigned int> trivial_buffer(vertice::vertex& layout, stn::array<float>& points) {
		array<unsigned int> trivial;
		size_t vertices = points.length / layout.stride();
		for (size_t i = 0; i < vertices; i++)
		{
			trivial.push(i);
		}
		return trivial;
	}

	struct MeshData {
		Ids::Id mesh;
		stn::array<float> pointlist;
		stn::array<unsigned int> indicelist;
		MeshData(Ids::Id msh,stn::array<float>&& pointlist,stn::array<unsigned int>&& indicelist ) :mesh(msh), indicelist(std::move(indicelist)), pointlist(std::move(pointlist))
		{


		}
		~MeshData() {

			indicelist.destroy();
			pointlist.destroy();
		}
		MeshData() {

		}
	};
	struct RenderableHandle {
		Ids::Id id;
		Renderer* renderer;
		RenderableHandle() {


		}
		RenderableHandle(Ids::Id id, Renderer* renderer)
			: id(id), renderer(renderer) {
		}

		void set_material(const std::string& name);
		
		void set_layout(vertice::vertex layout);
		
		void fill(stn::array<float>&& points, stn::array<unsigned int>&& indices);
		
		void set_uniform(const uniforms::uniform& u);
		
		void render();
		
		void remove();
		
		bool valid() const {
			return static_cast<bool>(id);
		}
	};
	struct Renderer {
		
		void Bind_Texture(Ids::Id Handle) {
			context.Bind( Textures.get_texture(Handle));

		}
		Renderer();
		void InitilizeBaseMaterials();	
		RenderContext::Context context;
		MaterialManager Modes;
		Material properties;
		void SetType(std::string Name);
		
		
		template<typename val_type>
		void set_uniform(const char* name,const val_type& val) {
		
			uniform_man.set(name,uniforms::uniform_val{val});

		}
		void apply_uniform(uniforms::uniform_val val, const char* location_in_shader);
		template <typename... Args>
		void Construct(const char* name, const char* shade,  RenderProperties props,Args&&...args) {
			 Modes.Construct(name,shade,props, std::forward<Args>(args)...);
		}
		
		ITexture* CurrentTexture() {
			return context.Get_BoundTexture();
		};
		shader* CurrentShader() {
			return context.Get_BoundShader();
		}
		void Clear();
		uniforms::UniformManager uniform_man;
		Shaders::ShaderManager Shaders;
		VObjMan::VObjManager Binders;
		TextureManager::TextureManager Textures;
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, const glm::vec2& vec);
		void SetUniform(const std::string& name, const glm::vec3& vec);
		void SetUniform(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4(const std::string& name, const glm::mat4& mat);
		void bind_material(Ids::Id material);
		meshId gen() {


		}
	
		void destroy(meshId mesh) {

		}
		
		void Gen(Mesh* mesh);
		void Destroy(Mesh* mesh);

		void Fill(Mesh* mesh, stn::array<float>& pointlist);
		void Fill(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist);
	
		
		void Render(Mesh* mesh);
		void Render(Mesh* mesh, stn::array<float>& pointlist);
		void Render(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist);
		void FillVertexBuffer(Mesh* mesh, stn::array<float>& pointlist);
		void setviewmatrix(glm::mat4 viewmat);
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
			for (auto i = 0; i < rend.overides.length; i++)
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
		void render(Ids::Id renderable_id) {

			renderable& value = renderables[renderable_id.id];
			Ids::Id mat_id = value.material;
			if (!mat_id)
			{
				throw std::logic_error("Material was not set");
			}
			Ids::Id mesh_id = value.mesh;
			bind_material(mat_id);
			for (auto& uniform :value.overides)
			{
				apply_uniform(uniform.value,uniform.name);
			}
			Render(&meshes[mesh_id]);
		}
		
		void fill_cmd(Ids::Id renderable_id, stn::array<float>&& pointlist, stn::array<unsigned int>&& indicelist) {
			Ids::Id mesh_id = ensure_mesh(renderable_id);
			to_fill.push(MeshData(mesh_id,  std::move(pointlist), std::move(indicelist)));
		}
		void fill_mesh(MeshData& data) {
			//must exist as a precurus as it is created through fillcmd;
			Ids::Id mesh_id = data.mesh;
			Fill(&meshes[mesh_id], data.pointlist, data.indicelist);
		}
		void set_layout(Ids::Id renderable_id, vertice::vertex layout) {
			Ids::Id mesh_id = ensure_mesh(renderable_id);
			meshes[mesh_id].Voa.attributes = layout;
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
		void consume() {
			while (!to_fill.empty())
			{
				fill_mesh(to_fill.peek());
				to_fill.pop();
			}
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

