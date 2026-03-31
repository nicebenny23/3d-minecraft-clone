#include "texture.h"
#include "shader.h"
#include "Uniform.h"
#include "../../util/variant.h"
#include "mesh.h"
#include "Fbo.h"
#include "vertex.h"
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

	using mesh_id = stn::rc<GpuMesh>;
	struct CpuMesh {
		mesh_id mesh;
		vertice::vertex layout;
		stn::array<float> points;
		stn::array<std::uint32_t> indices;

	};
	struct Context {
		void clear() {
			glClearColor(0, 0, 0, 0.0f);
			glDepthMask(GL_TRUE);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glDepthMask(GL_FALSE);
		}

		size_t frame_id() {
			return bound_frame.map([](Fbo frame) {return frame.id; }).unwrap_or(0);
		}
		void bind(GpuMesh& mesh) {
			if (mesh.vao.id == 0) {
				throw std::logic_error("mesh unitilized");
			}
			glBindVertexArray(mesh.vao.id);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo.id);
			glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo.id);
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
				[&](const int& val) {
					glad_glUniform1i(location, val);
				},
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



		GpuMesh create_mesh() {

			GLuint vao = 0, vbo = 0, ebo = 0;
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);
			GlUtil::poll_errors();
			return GpuMesh(
			0,
			Vao(vao),
			Ebo(ebo),
			Vbo(vbo));
		}

		void destroy(GpuMesh& msh) {
			glDeleteBuffers(1, &msh.ebo.id);
			glDeleteBuffers(1, &msh.vbo.id);
			glDeleteVertexArrays(1, &msh.vao.id);
			msh.length = 0;

		}
		void load(CpuMesh& data) {

			GpuMesh& msh = *data.mesh;
			if (data.points.length() % data.layout.components() != 0) {
				throw std::logic_error("Vertex Data is corrupted");
			}
			if (msh.ebo.id) {
				bind(msh);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.length() * sizeof(GLuint), data.indices.data(), GL_STATIC_DRAW);
				glBufferData(GL_ARRAY_BUFFER, data.points.length() * sizeof(float), data.points.data(), GL_STATIC_DRAW);
				size_t stride = data.layout.stride();
				size_t offset = 0;
				vertice::vertex& mesh_vertex = data.layout;
				for (int i = 0; i < mesh_vertex.length(); i++) {
					vertice::vertex_attribute& attribute = mesh_vertex[i];
					GlUtil::set_attr(i, attribute.components, attribute.type, GLsizei(stride), offset);
					glEnableVertexAttribArray(i);
					offset += attribute.size;
				}

				msh.length = data.indices.length();
			}
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
		stn::Option <GpuMesh> bound_mesh;


	};






}