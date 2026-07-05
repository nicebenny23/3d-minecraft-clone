#include "RenderContext.h"
#pragma once
namespace renderer {

	struct FboTextureLoader {
		using load_descriptor = FboTextureDescriptor;
		renderer::Context& context;
		FboTextureLoader(renderer::Context& ctx) :context(ctx) {

		}
		stn::box<Texture2D> load(const FboTextureDescriptor& desc) {
			GLuint id_number = context.gen_texture();
			Texture2D id(desc.size, id_number);
			context.bind(id);
			GLenum internalFormat, format, type;
			switch (desc.type) {
			case FboTextureDescriptor::Type::Color:
			internalFormat = GL_RGBA8;
			format = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
			case FboTextureDescriptor::Type::Integer:
			internalFormat = GL_R32I;
			format = GL_RED_INTEGER;
			type = GL_INT;
			break;
			case FboTextureDescriptor::Type::Depth:
			internalFormat = GL_DEPTH_COMPONENT24;
			format = GL_DEPTH_COMPONENT;
			type = GL_UNSIGNED_INT;
			break;

			}

			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, desc.size.x, desc.size.y, 0, format, type, nullptr);
			set_default_texture_paramaters(id.id, false);
			return stn::box<Texture2D>(id);
		}

		void unload(stn::box<Texture2D> tex) {
			glDeleteTextures(1, &tex->id);
			tex->id = 0;
		}
	};

	struct TextureLoader {
		renderer::Context& context;
		TextureLoader(renderer::Context& ctx) :context(ctx) {

		}
		using load_descriptor = renderer::TexturePath;
		stn::box<Texture2D> load(const renderer::TexturePath& path) {
			v2::Coord2 size;
			unsigned char* data = gl_util::texture_for(&size.x, &size.y, path.path.c_str());
			GLuint id_number = 0;
			glGenTextures(1, &id_number);
			Texture2D id = Texture2D(size, id_number);
			context.bind(id);
			GLint type = GL_RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, type, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			set_default_texture_paramaters(GL_TEXTURE_2D);
			return stn::box<Texture2D>(id);
		}
		void unload(stn::box<Texture2D> texture) {

			glDeleteTextures(1, &texture->id);
			texture->id = 0;
		}
	};

	

	struct TextureArrayLoader {
		renderer::Context& context;
		TextureArrayLoader(renderer::Context& ctx) :context(ctx) {
			int l = 3;
		}
		using load_descriptor = TextureArrayPath;
		stn::box<TextureArray> load(const TextureArrayPath& path) {

			v2::Coord2 size;
			if (path.count() != 0) {
				size = gl_util::image_size(path.paths[0].c_str());
			}

			GLuint id_number = context.gen_texture();
			TextureArray id = TextureArray(path.count(), size, id_number);

			context.bind(id);
			glTexImage3D(
				GL_TEXTURE_2D_ARRAY //type
				, 0                 //minmap level
				, GL_RGBA8            //color format
				, size.x, size.y, static_cast<GLsizei>(path.count())//size
				, 0,
				GL_RGBA,//color format
				GL_UNSIGNED_BYTE, nullptr);//arr
			array<unsigned char*> data = array<unsigned char*>();

			for (int i = 0; i < path.count(); i++) {
				//colchannel not used but it hastto be given
				v2::Coord2 sub_size;
				unsigned char* sub_texture = gl_util::texture_for(&sub_size.x, &sub_size.y, path[i].c_str());
				if (sub_size != size) {
					throw	std::logic_error("texture is invalid");
				}
				//params,type,minmap level,xoffset,offset,zoffset,width,heigh,depgh,colortype,datatype,and data;
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, size.x, size.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, sub_texture);

				stbi_image_free(sub_texture);
			}
			set_default_texture_paramaters(GL_TEXTURE_2D_ARRAY);
			return stn::box<TextureArray>(id);
		}
		void unload(stn::box<TextureArray> texture) {
			glDeleteTextures(1, &texture->id);
			texture->id = 0;
		}
	};


	struct FrameLoader {
		renderer::Context& ctx;
		FrameLoader(renderer::Context& context) :ctx(context) {

		}
		stn::box<Fbo> Load(const Frame& frame, assets::Assets& assets) {
			Fbo fbo = ctx.gen_fbo();
			ctx.bind(fbo);
			int color_attachment_index = 0;
			stn::array<GLenum> draw_buffers;
			GLenum texTarget = (frame.samples > 1) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;

			for (const FboTextureDescriptor& desc : frame.descriptors) {
				assets::AssetHandle<Texture2D> tex = assets.load<FboTextureDescriptor>(desc).expect("asset should exist");

				GLenum attachment;
				switch (desc.type) {
				case FboTextureDescriptor::Type::Color:
				case FboTextureDescriptor::Type::Integer:
				attachment = GL_COLOR_ATTACHMENT0 + color_attachment_index++;
				draw_buffers.push(attachment);
				break;

				case FboTextureDescriptor::Type::Depth:
				attachment = GL_DEPTH_ATTACHMENT;
				break;
				}
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, texTarget, tex->id, 0);
			}
			if (!draw_buffers.empty()) {
				glDrawBuffers(static_cast<GLsizei>(draw_buffers.length()), draw_buffers.data());
			}
			else {
				glDrawBuffer(GL_NONE);
			}

		}
		static constexpr bool immortal = true;
		void unload(stn::box<Frame>&& frame) {

		}
	};
	struct ShaderLoader {
		renderer::Context& context;
		using load_descriptor = shader_descriptor;
		ShaderLoader(renderer::Context& ctx) :context(ctx) {

		}
		void unload(stn::box<Shader> Shader) {

		}
		static constexpr bool immortal = true;
		stn::box<Shader> load(const shader_descriptor& load_info) {
			int VertexShader = compile_shader(load_info.vertex_path.c_str(), GL_VERTEX_SHADER);
			int FragmentShader = compile_shader(load_info.fragment_path.c_str(), GL_FRAGMENT_SHADER);
			GLint id = glCreateProgram();
			glAttachShader(id, VertexShader);
			glAttachShader(id, FragmentShader);
			glLinkProgram(id);
			gl_util::shaderstatuscheck(id, GL_LINK_STATUS, "shader program");
			glDeleteShader(VertexShader);
			glDeleteShader(FragmentShader);
			glValidateProgram(id);
			gl_util::poll_errors();
			return stn::box<Shader>(id);
		}
	};
}
