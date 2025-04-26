#pragma once
#include "..//renderer/renderer.h"
#include "Vao.h"
#include "vertexobject.h"
#include "shader.h"
#include <glm/mat4x4.hpp>
#include "../util/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include <unordered_map>
#include "ShaderManager.h"
#include "VertexObjectManager.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "RenderProperties.h"
using namespace vobj;

namespace renderer {

	
	struct Renderer {
		RenderMode properties;
		RenderModeManager Modes;
		void SetType(std::string Name);
		void AddType(const RenderMode& type) {
			Modes.AddType(type);
		}
		void applyProperties();
		Renderer() {
		}
		ITexture* CurrentTexture() {
			return Textures.CurrentTexture;
		};
		shader* CurrentShader() {
			return Shaders.BoundShader;
		}
		void Clear();
		Shaders::ShaderManager Shaders;
		VObjMan::VObjManager Binders;
		TextureManager::TextureManager Textures;
		void SetUniform(const std::string& name, float value);
		void SetUniform(const std::string& name, const glm::vec2& vec);
		void SetUniform(const std::string& name, const glm::vec3& vec);
		void SetUniform(const std::string& name, const glm::vec4& vec);
		void SetUniformMat4(const std::string& name, const glm::mat4& mat);


		template <bool HasEbo>
		void Gen(Mesh* mesh);
		void Destroy(Mesh* mesh);

		void Fill(Mesh* mesh, dynamicarray::array<float>& pointlist);
		void Fill(Mesh* mesh, dynamicarray::array<float>& pointlist, dynamicarray::array<unsigned int>& indicelist);

		void Render(Mesh* mesh);
		void Render(Mesh* mesh, dynamicarray::array<float>& pointlist);
		void Render(Mesh* mesh, dynamicarray::array<float>& pointlist, dynamicarray::array<unsigned int>& indicelist);


		void setaspectratio();	
		void FillVertexBuffer(Mesh* mesh, dynamicarray::array<float>& pointlist);
	};
	extern Renderer Ren;

	void load();
	void setviewmatrix(glm::mat4 viewmat);

	extern glm::mat4 proj,view;
	
	extern float fov;
	
	void setrenderingmatrixes();
	void setaspectratio();
	
	enum rendertype
	{
		renderui=0,
		rendertext = 1,
		rendermodel= 2,
		renderparticle=3,
	
	}; 
	extern rendertype currendertype;
		void changerendertype(rendertype rentype);

		template<bool HasEbo>
		inline void Renderer::Gen(Mesh* mesh)
		{
			mesh->IsEboMesh = HasEbo;
			if (mesh->IsEboMesh)
			{
			Binders.Create(&mesh->Ibo);
			}
			Binders.Create(&mesh->Voa);
			Binders.Create(&mesh->Vbo);
			mesh->BuffersGenerated = true;
		}

}

