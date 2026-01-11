#include "../../math/vector2.h"
#include <GLFW/glfw3.h>
#include "../../util/dynamicarray.h"
#include <glad/glad.h>
#include "../../util/unique.h"
#include "shader.h"
#include "../../util/stbiload.h"
#include "../../external/stb_image.h"
#pragma once 
using namespace stn;
namespace renderer {
	enum TextureType {
		TexArray = GL_TEXTURE_2D_ARRAY,
		Tex2d = GL_TEXTURE_2D,
		Tex3d = 2,
		TextureNull = 3,
	}; namespace TextureManager {
		class TextureManager; // Forward declaration
	}

	struct Texture2D :assets::Asset {
		Texture2D() = default;
		v2::Coord2 size;
		GLuint id;
		void apply() const {

			glBindTexture(GL_TEXTURE_2D_ARRAY, id);
		}
		void destroy() {

		}
		Texture2D(v2::Coord2 size, GLuint id) :size(size), id(id) {
		}

	};

	struct TexturePath {
		using asset_type = Texture2D;
		std::string path;
		std::string name;
		TexturePath(const std::string& path, const std::string& name) :path(path), name(name) {
		}
		TexturePath(const char* path, const char* name) :path(path), name(name) {
		}
		bool operator==(const TexturePath& other) const noexcept {
			return path == other.path
				&& name == other.name;
		}

	};
}
namespace std {
	template<>
	struct hash<renderer::TexturePath> {
		std::size_t operator()(const renderer::TexturePath& s) const noexcept {
			std::hash<std::string> hasher;
			return hasher(s.name) + hasher(s.path);
		}
	};
}
namespace renderer {
	inline void settextureparams(GLint texturetype, bool with_minmaps = true) {

		if (with_minmaps) {
			glGenerateMipmap(texturetype);
		}
		glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	}
	struct TextureLoader {
		TextureLoader() {

		}
		using load_descriptor = renderer::TexturePath;
		stn::box<Texture2D> load(const renderer::TexturePath& path) {
			v2::Coord2 size;
			unsigned char* data = texdata::loadtexdata(&size.x, &size.y, path.path.c_str());
			GLuint id = 0;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			GLint type = GL_RGBA;
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, size.x, size.y, 0, type, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
			settextureparams(GL_TEXTURE_2D);
			return stn::box<Texture2D>(size, id);
		}
		void unload(stn::box<Texture2D> texture) {

			glDeleteTextures(1, &texture->id);
			texture->id = 0;
		}
	};
	struct TextureArray :assets::Asset {
		TextureArray() = default;
		size_t length;
		v2::Coord2 size;
		GLuint id;
		void apply() const {
			if (id == 0) {
				throw std::logic_error("cant apply invalid texture");
			}
			glBindTexture(GL_TEXTURE_2D, id);
		}
		void destroy() {
			if (id == 0) {
				throw std::logic_error("cant Delete Invalid Texture");
			}
			glDeleteTextures(1, &id);
			id = 0;

		}

		TextureArray(size_t len, v2::Coord2 image_size, GLuint gl_id) :id(gl_id), size(image_size), length(len) {
		}
	};

	struct TextureArrayPath {
		using asset_type = TextureArray;
		stn::array<std::string> paths;
		std::string name;
		size_t count() const {
			return paths.length();
		}
		const std::string& operator[](size_t index) const {
			return paths[index];
		}
		bool operator==(const TextureArrayPath& other) const {
			return name == other.name && paths == other.paths;
		}
		TextureArrayPath(const stn::array<std::string>& paths, const std::string& name) :paths(paths), name(name) {
		}
	};
}
namespace std {
	template<>
	struct hash<renderer::TextureArrayPath> {
		std::size_t operator()(const renderer::TextureArrayPath& s) const noexcept {
			std::hash<std::string> hasher;
			std::hash<stn::array<std::string>> path_hasher;
			return hasher(s.name) + path_hasher(s.paths);
		}
	};
}
namespace renderer {

	struct TextureArrayLoader {
		TextureArrayLoader() {

		}
		using load_descriptor = TextureArrayPath;
		stn::box<TextureArray> load(const TextureArrayPath& path) {

			GLuint id = 0;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D_ARRAY, id);
			v2::Coord2 size;
			if (path.count() != 0) {
				size = texdata::GetImgSize(path.paths[0].c_str());
			}


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
				v2::Coord2 ImgSize;
				unsigned char* datatoappend = texdata::loadtexdata(&ImgSize.x, &ImgSize.y, path[i].c_str());
				if (ImgSize != size) {
					throw	std::logic_error("texture is invalid");
				}
				//params,type,minmap level,xoffset,offset,zoffset,width,heigh,depgh,colortype,datatype,and data;
				glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, size.x, size.y, 1, GL_RGBA, GL_UNSIGNED_BYTE, datatoappend);

				stbi_image_free(datatoappend);
			}
			settextureparams(GL_TEXTURE_2D_ARRAY);
			return stn::box<TextureArray>(path.count(), size, id);
		}
		void unload(stn::box<TextureArray> texture) {

			glDeleteTextures(1, &texture->id);
			texture->id = 0;
		}
	};


	struct FboTextureDescriptor {
		using asset_type = Texture2D;
		enum class Type {
			Color,
			Depth,
			Integer,
		};
		Type type = Type::Color;
		bool sampleable = true;
		FboTextureDescriptor(Type ty = Type::Color, bool is_sampleable = true)
			: type(ty), sampleable(is_sampleable) {
		}
		std::string name;
		bool operator==(const FboTextureDescriptor& other) const = default;
		v2::Coord2 size;
	};

}
namespace std {
	template<>
	struct hash<renderer::FboTextureDescriptor> {
		std::size_t operator()(const renderer::FboTextureDescriptor& s) const noexcept {
			return std::hash<std::string>()(s.name);
		}
	};

}
namespace renderer {
	struct FboTextureLoader {
		using load_descriptor = FboTextureDescriptor;

		stn::box<Texture2D> load(const FboTextureDescriptor& desc) {
			GLuint id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);

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
			settextureparams(id, false);
			return stn::box<Texture2D>(desc.size, id);
		}

		void unload(stn::box<Texture2D> tex) {
			glDeleteTextures(1, &tex->id);
			tex->id = 0;
		}
	};


	using texture_2d_id = assets::AssetHandle<Texture2D>;
	using texture_array_id = assets::AssetHandle<TextureArray>;
};