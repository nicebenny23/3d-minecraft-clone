#pragma once
#include "RenderContext.h"

namespace renderer {


	void Context::bind(Mesh& mesh)
	{
		glBindVertexArray(mesh.vao_id());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo_id());
		glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id());
		bound_mesh = mesh;
	}

	void Context::unbind_mesh()
	{

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		bound_mesh = stn::None;
	}

	void Context::bind(ITexture& Tex)
	{

		glBindTexture(Tex.type, Tex.id);
		BoundTexture = Tex;


	}




	void Context::bind(shader& Shader)
	{
		if (Shader.id == 0)
		{
			throw std::logic_error("cant attach invalid shader");
		}
		if (BoundShader.is_none_or([&Shader](const shader& shader_id) {return shader_id.id != Shader.id; }))
		{
		glUseProgram(Shader.id);
			BoundShader = stn::Option<shader&>(Shader);
		}
	}
	void Context::unbind_shader()
	{
		glUseProgram(0);
		BoundShader = stn::None;
	}



	void Context::unbind_texture()
	{
		
		glBindTexture(BoundTexture.unwrap().type, 0);
		BoundTexture = stn::None;
	}

	void Context::bind_properties(const RenderProperties& props) {
		glDepthMask(props.depthWriteEnabled);
		GlUtil::SetProperty(GL_CULL_FACE, props.cullFaceEnabled);
		GlUtil::SetProperty(GL_DEPTH_TEST, props.depthTestEnabled);
		glBlendFunc(props.blendFuncSrc, props.blendFuncDst);
		GlUtil::SetProperty(GL_BLEND, props.blendingEnabled);

	}

	Mesh Context::create() {
		Mesh new_mesh = Mesh();
		glGenVertexArrays(1, &new_mesh.Voa.id);
		glGenBuffers(1, &new_mesh.Vbo.Buf.id);
		glGenBuffers(1, &new_mesh.Ibo.Buf.id);
		new_mesh.BuffersGenerated = true;
		return new_mesh;
	}
	void Context::destroy(Mesh& msh) {
		if (!msh.generated())
		{
			throw std::invalid_argument("Cannot Delete a mesh without Generating buffers first");
		}

		glDeleteBuffers(1, &msh.Ibo.Buf.id);
		glDeleteBuffers(1, &msh.Vbo.Buf.id);
		glDeleteVertexArrays(1, &msh.Voa.id);
		msh.BuffersGenerated = false;
		msh.length = 0;

	}
}