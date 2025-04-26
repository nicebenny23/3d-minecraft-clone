#include "Vao.h"
#include "vertexobject.h"
#include "texture.h"
#include "shader.h"
#include "../Resources/ResourceManager.h"
#pragma once
//Context For the Renderer which Controls Bindings And State
namespace RenderContext{

	//Context For the Renderer which Controls Bindings And State
	struct Context
	{

		void Bind(vobj::Vbo* Vbo);
		void Bind(vobj::Ebo* Ebo);
		void Bind(VaoName::Vao* Vao);
		void Bind(ITexture* Tex);
		void Bind(shader* Shader);
		void UnbindVbo();
		void UnbindVao();
		void UnbindEbo();
		void UnbindTexture();
		void UnbindShader();
	private:

		shader* BoundShader;
		ITexture* BoundTexture;
		vobj::Ebo* BoundEbo;
		vobj::Vbo* BoundVbo;
		VaoName::Vao* BoundVao;


	};






}