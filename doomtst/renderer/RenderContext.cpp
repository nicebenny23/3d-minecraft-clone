#include "RenderContext.h"

void RenderContext::Context::Bind(vobj::Vbo* Vbo)
{
	glBindBuffer(GL_ARRAY_BUFFER, Vbo->Getid());
	BoundVbo = Vbo;
}

void RenderContext::Context::Bind(vobj::Ebo* Ebo)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,Ebo->Getid());
	BoundEbo = Ebo;
}

void RenderContext::Context::Bind(VaoName::Vao* Vao)
{
	glBindVertexArray(Vao->id);
	BoundVao = Vao;
}

void RenderContext::Context::Bind(ITexture* Tex)
{
	glBindTexture(Tex->type, Tex->id);
	BoundTexture = Tex;
}

void RenderContext::Context::Bind(shader* Shader)
{
	
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
	
	BoundTexture = nullptr;
}

void RenderContext::Context::UnbindShader()
{
	
	BoundShader = nullptr;
}
