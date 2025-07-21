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
		Mesh* msh;
		stn::array<float> pointlist; 
		stn::array<unsigned int> indicelist;
		MeshData(Mesh* mesh, stn::array<unsigned int>&& indicelist, stn::array<float>&& pointlist):msh(mesh),indicelist(std::move(indicelist)),pointlist(std::move(pointlist))
		{


		}
		MeshData(Mesh* mesh, stn::array<float>&& pointlist) :msh(mesh), pointlist(std::move(pointlist))
		{
			indicelist = trivial_buffer(msh->Voa.attributes, pointlist);
		}
		~MeshData() {
			
			indicelist.destroy();
			pointlist.destroy();
		}
		MeshData() {

			msh = nullptr;
		}
	};
	struct MeshData2 {
		Ids::GenId mesh;
		vertice::vertex layout;
		stn::array<float> pointlist;
		stn::array<unsigned int> indicelist;
		MeshData2(vertice::vertex& lay, stn::array<unsigned int>&& indicelist, stn::array<float>&& pointlist) :layout(lay), indicelist(std::move(indicelist)), pointlist(std::move(pointlist))
		{


		}
		MeshData2(vertice::vertex& lay, stn::array<float>&& pointlist) :layout(lay), pointlist(std::move(pointlist))
		{
			indicelist = trivial_buffer(lay, pointlist);
		}
		~MeshData2() {

			indicelist.destroy();
			pointlist.destroy();
		}
		MeshData2() {

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
		void apply_uniform(uniforms::uniform_ref uform, const char* location_in_shader);
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
		void bind_material(size_t material);
		meshId gen() {


		}
		void gen_renderable() {



		}
		void destroy(meshId mesh) {

		}
		
		void Gen(Mesh* mesh);
		void Destroy(Mesh* mesh);

		void Fill(Mesh* mesh, stn::array<float>& pointlist);
		void Fill(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist);
		void CommandFill(MeshData& data) {
			Fill(data.msh, data.pointlist, data.indicelist);
		}
		void CommandRender(MeshData& data) {
			Render(data.msh, data.pointlist,data.indicelist);
		}
		stn::stack<MeshData> stck;
		void Consume() {

			while (!stck.empty())
			{
				CommandFill(stck.peek());
				stck.pop();
			}
		}

		
		void Render(Mesh* mesh);
		void Render(Mesh* mesh, stn::array<float>& pointlist);
		void Render(Mesh* mesh, stn::array<float>& pointlist, stn::array<unsigned int>& indicelist);
		void FillVertexBuffer(Mesh* mesh, stn::array<float>& pointlist);
		void setviewmatrix(glm::mat4 viewmat);
		void setprojmatrix(float newfov, float nearclipplane, float farclipplane);
	

		float fov;
	};

	
		inline void Renderer::Gen(Mesh* mesh)
		{
		
			
			Binders.Create(&mesh->Ibo);
			
			Binders.Create(&mesh->Voa);
			Binders.Create(&mesh->Vbo);
			mesh->BuffersGenerated = true;
		}

}

