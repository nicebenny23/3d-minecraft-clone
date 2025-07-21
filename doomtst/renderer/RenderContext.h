#include "Vao.h"
#include "vertexobject.h"
#include "texture.h"
#include "shader.h"
#include "../Resources/ResourceManager.h"
#include "RenderProperties.h"
#include "ShaderManager.h"
#pragma once
namespace renderer {
	struct Renderer;
}
//Context For the Renderer which Controls Bindings And State
namespace RenderContext{

	//Context For the Renderer which Controls Bindings And State
	struct Context
	{

		void Bind(buffer_object::Vbo& Vbo);
		void Bind(buffer_object::Ebo& Ebo);
		void Bind(VaoName::Vao& Vao);
		void Bind(ITexture& Tex);
		void Bind(shader& Shader);
		void UnbindVbo();
		void UnbindVao();
		void UnbindEbo();
		void UnbindTexture();
		void UnbindShader();
		shader* Get_BoundShader() {
			return BoundShader;
		}
		ITexture* Get_BoundTexture()
		{
			return BoundTexture;
		}
		buffer_object::Ebo* Get_BoundEbo() {
			return BoundEbo;
		}
		buffer_object::Vbo* Get_BoundVbo() {
			return BoundVbo;
		}
		VaoName::Vao* Get_BoundVao() {
			return BoundVao;
		}
		shader& bound_shader() {

			return *BoundShader;
		}
		void bind_properties(const RenderProperties& props);
		Context() {}
	
	private:
		const Material * material;
		shader* BoundShader;
		ITexture* BoundTexture;
		buffer_object::Ebo* BoundEbo;
		buffer_object::Vbo* BoundVbo;
		VaoName::Vao* BoundVao;
	

	};






}