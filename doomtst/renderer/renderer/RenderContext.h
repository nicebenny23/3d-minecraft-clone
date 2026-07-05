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

		bool depth_test = true;     // Enable/Disable Depth Test
		bool depth_write = true;    // Enable/Disable Depth Write
		bool blend = false;     // Enable/Disable Blending
		bool cull = true;      // Enable/Disable Face Culling
		GLenum blend_function_source = GL_SRC_ALPHA;   // Source blend function
		GLenum blend_function_distance = GL_ONE_MINUS_SRC_ALPHA; // Destination blend function
		GLenum draw_mode=GL_TRIANGLES;
		// Default constructor
		RenderProperties() = default;

		// Constructor with parameters to set initial properties
		RenderProperties(bool depthTest, bool depthWrite, bool Cull, bool Blending, GLenum BlendFunctionSrc, GLenum BlendFunctionDst, GLenum draw_shape_mode= GL_TRIANGLES)
			: depth_test(depthTest),
			depth_write(depthWrite),
			blend(Blending),
			cull(Cull),
			blend_function_source(BlendFunctionSrc),
			blend_function_distance(BlendFunctionDst), draw_mode(draw_shape_mode){
		}
		bool operator==(const RenderProperties& other) const = default;

	};
	using MeshId = stn::arc<GpuMesh>;
	struct CpuMesh {
		MeshId mesh;
		renderer::vertex layout;
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
			bound_texture_2d = Tex;
		}
		GLuint gen_texture() {
			GLuint id = 0;
			glGenTextures(1, &id);
			return id;
		}
		Fbo gen_fbo() {
			GLuint id = 0;
			glGenFramebuffers(1, &id);
			return Fbo(id);
		}
		void bind(const TextureArray& Tex) {
			glBindTexture(GL_TEXTURE_2D_ARRAY, Tex.id);
			bound_texture_array = Tex;
		}
		void bind(const Shader& shade) {
			if (shade.id == 0) {
				throw std::logic_error("cant attach invalid shader");
			}
			if (bound_shader.is_none_or([&shade](const Shader& shader_id) {return shader_id.id != shade.id; })) {
				glUseProgram(shade.id);
				bound_shader = stn::Option<Shader>(shade);
			}
		}

		void bind(Fbo fbo) {
			glBindFramebuffer(GL_FRAMEBUFFER, fbo.id);
			bound_frame = fbo;
		}
		void draw(GpuMesh mesh) {
			bind(mesh);
			glDrawElements(properties.draw_mode, mesh.length, GL_UNSIGNED_INT, 0);
		}

		void unbind_mesh() {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			bound_mesh = stn::None;
		}

		void unbind_texture_2d() {
			glBindTexture(GL_TEXTURE_2D, 0);
			bound_texture_2d = stn::None;
		}

		void unbind_texture_array() {
			glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
			bound_texture_array = stn::None;
		}
		void unbind_shader() {
			glUseProgram(0);
			bound_shader = stn::None;
		}
		void bind_to_screen() {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			bound_frame = 0;
		}
		void apply_uniform(const renderer::uniform_value& value, std::string_view name) {
			GLint location = bound_shader.unwrap().uniformlocation(name.data());
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
				[&](const Texture2dId& tex) {bind(*tex); },
				[&](const TextureArrayId& tex) {bind(*tex); });
			std::visit(visitor, value);
		}



		GpuMesh insert_builder_for() {

			GLuint vao = 0, vbo = 0, ebo = 0;

			gl_util::poll_errors();
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
			glGenBuffers(1, &ebo);
			gl_util::poll_errors();
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
				renderer::vertex& mesh_vertex = data.layout;
				for (int i = 0; i < mesh_vertex.length(); i++) {
					renderer::vertex_attribute& attribute = mesh_vertex[i];
					gl_util::set_attr(i, attribute.components, attribute.type, GLsizei(stride), offset);
					glEnableVertexAttribArray(i);
					offset += attribute.size;
				}

				msh.length = data.indices.length();
			}
		}
		Shader get_bound_shader() {
			return bound_shader.unwrap();
		}

		void bind_properties(const RenderProperties& props) {
			properties = props;
			glDepthMask(props.depth_write);
			gl_util::set_property(GL_CULL_FACE, props.cull);
			gl_util::set_property(GL_DEPTH_TEST, props.depth_test);
			glBlendFunc(props.blend_function_source, props.blend_function_distance);
			gl_util::set_property(GL_BLEND, props.blend);
		}
		Context() :bound_shader(), bound_texture_2d(), bound_texture_array(), bound_mesh() {
		}

	private:
		RenderProperties properties;
		stn::Option<Shader> bound_shader;
		stn::Option <Texture2D> bound_texture_2d;
		stn::Option <TextureArray> bound_texture_array;
		stn::Option<Fbo> bound_frame;
		stn::Option <GpuMesh> bound_mesh;


	};






}