#pragma once
#include "Vao.h"
#include "vertexobject.h"
#include "shader.h"
#include <glm/mat4x4.hpp>
#include "../util/vector3.h"
#include <glm/gtc/matrix_transform.hpp>
#include "texture.h"
#include "ShaderManager.h"
#include "VertexObjectManager.h"
#include "TextureManager.h"
#include "Mesh.h"
#include "RenderProperties.h"
#include "RenderContext.h"
using namespace buffer_object;
//Fix
namespace renderer {
	void setrenderingmatrixes(renderer::Renderer* renderer);
	void setAspectRatio(renderer::Renderer* renderer);
	struct Renderer {
		Renderer(size_t tst);
		RenderContext::Context context;
		Base_Material properties;
		RenderModeManager Modes;
		void SetType(std::string Name);
		void AddType(const Base_Material& type) {
			Modes.AddType(type);
		}
		void applyProperties();
		Renderer():Textures(),Binders(),Shaders() {

		}
		ITexture* CurrentTexture() {
			return context.Get_BoundTexture();
		};
		shader* CurrentShader() {
			return context.Get_BoundShader();
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

		void Fill(Mesh* mesh, Cont::array<float>& pointlist);
		void Fill(Mesh* mesh, Cont::array<float>& pointlist, Cont::array<unsigned int>& indicelist);

		void Render(Mesh* mesh);
		void Render(Mesh* mesh, Cont::array<float>& pointlist);
		void Render(Mesh* mesh, Cont::array<float>& pointlist, Cont::array<unsigned int>& indicelist);


		void FillVertexBuffer(Mesh* mesh, Cont::array<float>& pointlist);

		void setviewmatrix(glm::mat4 viewmat);
		void setprojmatrix(float newfov, float nearclipplane, float farclipplane);
		glm::mat4 proj, view;

		float fov;
	};

	

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

