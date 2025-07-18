#pragma once
#include "RenderContext.h"

void RenderContext::Context::Bind(buffer_object::Vbo& Vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, Vbo.Getid());
	BoundVbo = &Vbo;
}

void RenderContext::Context::Bind(buffer_object::Ebo& Ebo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Ebo.Getid());
	BoundEbo = &Ebo;
}

void RenderContext::Context::Bind(VaoName::Vao& Vao)
{
	glBindVertexArray(Vao.id);
	BoundVao = &Vao;
}

void RenderContext::Context::Bind(ITexture& Tex)
{
	
		glBindTexture(Tex.type, Tex.id);
		BoundTexture = &Tex;
	
	
}

void RenderContext::Context::Bind(shader& Shader)
{
	if (Shader.id==0)
	{
		throw std::logic_error("cant attach invalid shader");
	}
	glUseProgram(Shader.id);
	BoundShader = &Shader;
}


void RenderContext::Context::UnbindVbo()
{

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	BoundVbo = nullptr;
}

void RenderContext::Context::UnbindVao()
{
	glBindVertexArray(0);
	BoundVao = nullptr;
}

void RenderContext::Context::UnbindEbo()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	BoundEbo = nullptr;
}

void RenderContext::Context::UnbindTexture()
{
	glBindTexture(BoundTexture->type,0);
	BoundTexture = nullptr;
}

void RenderContext::Context::UnbindShader()
{
	glUseProgram(0);
	BoundShader = nullptr;
}

void Apply_properties(const RenderProperties* props) {
	glDepthMask(props->depthWriteEnabled);
	GlUtil::SetProperty(GL_CULL_FACE, props->cullFaceEnabled);
	GlUtil::SetProperty(GL_DEPTH_TEST, props->depthTestEnabled);
	glBlendFunc(props->blendFuncSrc, props->blendFuncDst);
	GlUtil::SetProperty(GL_BLEND, props->blendingEnabled);

}

void RenderContext::Context::bind(const Base_Material* mat, renderer::Renderer* renderer)
{
	if (material==mat)
	{
		return;
	}
	Bind(*mat->shade);

	for (int i = 0; i < mat->UniformsCalls.length; i++)
	{
		mat->UniformsCalls[i](renderer);
	}
	Apply_properties(&mat->prop);
	material = mat;
}