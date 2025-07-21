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

void RenderContext::Context::bind(const Material* mat, renderer::Renderer* renderer)
{
	if (material==mat)
	{
		return;
	}
	Bind(*mat->shade);

	for (int i = 0; i < mat->handles.length; i++)
	{
		apply(mat->handles[i],mat->handles[i].name);
	}
	Apply_properties(&mat->prop);
	material = mat;
}

void RenderContext::Context::apply(uniforms::uniform_ref uform,const char* location_in_shader)
{
	uniforms::uniform_val val = form_man->get(uform);
	
		switch (val.index()) {
		case uniforms::uform_int:

			break;
		case uniforms::uform_float:
			BoundShader->setf(std::get<float>(val), location_in_shader);
			break;
		case uniforms::uform_gluint:
			// handle GLuint
			break;
		case uniforms::uform_vec2:
			BoundShader->SetVector2f(std::get<glm::vec2>(val), location_in_shader);
			break;
		case uniforms::uform_vec3:

			BoundShader->SetVector3f(std::get<glm::vec3>(val), location_in_shader);
			break;
		case uniforms::uform_vec4:
			BoundShader->SetVector4f(std::get<glm::vec4>(val), location_in_shader);
			break;
		case uniforms::uform_mat3:
			BoundShader->setMat3(std::get<glm::mat3>(val), location_in_shader);
			break;
		case uniforms::uform_mat4:
			BoundShader->setMat4(std::get<glm::mat4>(val), location_in_shader);
			break;
		case uniforms::uform_texarr:
			std::get<TextureArray*>(val)->apply();
			break;
		case uniforms::uform_tex2d:
			std::get<Texture2D*>(val)->apply();
			break;
		default:

			break;
		}
	
}
