#include "texture.h"
#include "shader.h"
#include "Uniform.h"
#include "../../util/variant.h"
#include "mesh.h"
#include "Fbo.h"
#pragma once
namespace renderer {
	struct Renderer;
}
//Context For the Renderer which Controls Bindings And State
namespace renderer {

	//Context For the Renderer which Controls Bindings And State
	class RenderProperties {
	public:
		bool depthTestEnabled = true;     // Enable/Disable Depth Test
		bool depthWriteEnabled = true;    // Enable/Disable Depth Write
		bool blendingEnabled = false;     // Enable/Disable Blending
		bool cullFaceEnabled = true;      // Enable/Disable Face Culling
		GLenum blendFuncSrc = GL_SRC_ALPHA;   // Source blend function
		GLenum blendFuncDst = GL_ONE_MINUS_SRC_ALPHA; // Destination blend function

		// Default constructor
		RenderProperties() = default;

		// Constructor with parameters to set initial properties
		RenderProperties(bool depthTest, bool depthWrite, bool Cull, bool Blending, GLenum BlendFunctionSrc, GLenum BlendFunctionDst)
			: depthTestEnabled(depthTest),
			depthWriteEnabled(depthWrite),
			blendingEnabled(Blending),
			cullFaceEnabled(Cull),
			blendFuncSrc(BlendFunctionSrc),
			blendFuncDst(BlendFunctionDst) {
		}
		bool operator==(const RenderProperties& other) const = default;

	};
	struct Context {


		size_t frame_id() {
			return bound_frame.map([](Fbo frame) {return frame.id; }).unwrap_or(0);
		}
		void bind(Mesh& mesh) {
			glBindVertexArray(mesh.vao_id());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo_id());
			glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_id());
			bound_mesh = mesh;

		}
		void bind(const Texture2D& Tex) {
			glBindTexture(GL_TEXTURE_2D, Tex.id);
			BoundTexture2d = Tex;
		}

		void bind(const TextureArray& Tex) {
			glBindTexture(GL_TEXTURE_2D_ARRAY, Tex.id);
			BoundTextureArray = Tex;
		}
		void bind(const shader& Shader) {
			if (Shader.id == 0) {
				throw std::logic_error("cant attach invalid shader");
			}
			if (BoundShader.is_none_or([&Shader](const shader& shader_id) {return shader_id.id != Shader.id; })) {
				glUseProgram(Shader.id);
				BoundShader = stn::Option<shader>(Shader);
			}
		}

		void bind(Fbo fbo) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
			bound_frame = fbo;
		}


		void unbind_mesh() {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			bound_mesh = stn::None;
		}

		void unbind_texture_2d() {
			glBindTexture(GL_TEXTURE_2D, 0);
			BoundTexture2d = stn::None;
		}

		void unbind_texture_array() {
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
			BoundTextureArray = stn::None;
		}
		void unbind_shader() {
			glUseProgram(0);
			BoundShader = stn::None;
		}
		void bind_to_screen() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			bound_frame = 0;
		}
		void apply_uniform(const renderer::uniform_value& value, std::string_view name) {
			GLint location = BoundShader.unwrap().uniformlocation(name.data());
			auto visitor = stn::visitor(
				[&](const int& val) {glad_glUniform1i(location, val); },
				[&](const float& val) {glad_glUniform1f(location, val); },
				[&](const unsigned int& val) {glad_glUniform1ui(location, val); },
				[&](const v2::Vec2& val) {
					glad_glUniform2f(location, static_cast<GLfloat>(val.x), static_cast<GLfloat>(val.y));
				},
				[&](const v3::Vec3& val) {
					glad_glUniform3f(location, static_cast<GLfloat>(val.x), static_cast<GLfloat>(val.y), static_cast<GLfloat>(val.z));
				},
				[&](const glm::vec4& val) {
					glad_glUniform4f(location, val.x, val.y, val.z, val.w);
				},
				[&](const glm::mat3& val) {
					glad_glUniformMatrix3fv(location, 1, GL_FALSE, &val[0][0]);
				},
				[&](const glm::mat4& val) {
					glad_glUniformMatrix4fv(location, 1, GL_FALSE, &val[0][0]);
				},
				[&](const texture_2d_id& tex) {bind(*tex); },
				[&](const texture_array_id& tex) {bind(*tex); });
			std::visit(visitor, value);
		}




		Mesh create_mesh() {
			Mesh new_mesh = Mesh();
			glGenVertexArrays(1, &new_mesh.Voa.id);
			glGenBuffers(1, &new_mesh.Vbo.Buf.id);
			glGenBuffers(1, &new_mesh.Ibo.Buf.id);
			new_mesh.BuffersGenerated = true;
			return new_mesh;
		}
		void destroy(Mesh& msh) {
			if (!msh.generated()) {
				throw std::invalid_argument("Cannot Delete a mesh without Generating buffers first");
			}

			glDeleteBuffers(1, &msh.Ibo.Buf.id);
			glDeleteBuffers(1, &msh.Vbo.Buf.id);
			glDeleteVertexArrays(1, &msh.Voa.id);
			msh.BuffersGenerated = false;
			msh.length = 0;

		}

		shader bound_shader() {
			return BoundShader.unwrap();
		}

		void bind_properties(const RenderProperties& props) {
			glDepthMask(props.depthWriteEnabled);
			GlUtil::SetProperty(GL_CULL_FACE, props.cullFaceEnabled);
			GlUtil::SetProperty(GL_DEPTH_TEST, props.depthTestEnabled);
			glBlendFunc(props.blendFuncSrc, props.blendFuncDst);
			GlUtil::SetProperty(GL_BLEND, props.blendingEnabled);
		}
		Context() :BoundShader(), BoundTexture2d(), BoundTextureArray(), bound_mesh() {
		}

	private:
		RenderProperties properties;
		stn::Option<shader> BoundShader;
		stn::Option <Texture2D> BoundTexture2d;
		stn::Option <TextureArray> BoundTextureArray;
		stn::Option<Fbo> bound_frame;
		stn::Option <Mesh> bound_mesh;


	};






}