#pragma once
#include "RenderContext.h"
namespace RenderContext {
	void Context::Bind(buffer_object::Vbo& Vbo)
	{
		glBindBuffer(GL_ARRAY_BUFFER, Vbo.Getid());
		BoundVbo = &Vbo;
	}

	void Context::Bind(buffer_object::Ebo& Ebo)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, Ebo.Getid());
		BoundEbo = &Ebo;
	}

	void Context::Bind(VaoName::Vao& Vao)
	{
		glBindVertexArray(Vao.id);
		BoundVao = &Vao;
	}

	void Context::Bind(ITexture& Tex)
	{

		glBindTexture(Tex.type, Tex.id);
		BoundTexture = &Tex;


	}

	void Context::Bind(shader& Shader)
	{
		if (Shader.id == 0)
		{
			throw std::logic_error("cant attach invalid shader");
		}Shader.attach();
		BoundShader = &Shader;
	}


	void Context::UnbindVbo()
	{

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		BoundVbo = nullptr;
	}

	void Context::UnbindVao()
	{
		glBindVertexArray(0);
		BoundVao = nullptr;
	}

	void Context::UnbindEbo()
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		BoundEbo = nullptr;
	}

	void Context::UnbindTexture()
	{
		glBindTexture(BoundTexture->type, 0);
		BoundTexture = nullptr;
	}

	void Context::UnbindShader()
	{
		glUseProgram(0);
		BoundShader = nullptr;
	}

	void Context::bind_properties(const RenderProperties& props) {
		glDepthMask(props.depthWriteEnabled);
		GlUtil::SetProperty(GL_CULL_FACE, props.cullFaceEnabled);
		GlUtil::SetProperty(GL_DEPTH_TEST, props.depthTestEnabled);
		glBlendFunc(props.blendFuncSrc, props.blendFuncDst);
		GlUtil::SetProperty(GL_BLEND, props.blendingEnabled);

	}

}