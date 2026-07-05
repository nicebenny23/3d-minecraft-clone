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


	struct Texture2D :assets::Asset {
		v2::Coord2 size;
		GLuint id;
		Texture2D(v2::Coord2 size, GLuint id) :size(size), id(id) {
		}

	};
	struct TexturePath {
		using asset_type = Texture2D;
		std::string path;
		std::string_view name() const {
			return path;
		};
		TexturePath(const std::string& path) :path(path) {

		}

		TexturePath(const char* path) :path(path) {
		}
		bool operator==(const TexturePath& other) const noexcept {
			return path == other.path;
		}

	};
	inline void set_default_texture_paramaters(GLint texturetype, bool with_minmaps = true) {

		if (with_minmaps) {
			glGenerateMipmap(texturetype);
		}
		glEnable(GL_MULTISAMPLE);
		glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(texturetype, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(texturetype, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	}


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

	struct TextureArray :assets::Asset {
		size_t length;
		v2::Coord2 size;
		GLuint id;
		
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

	using Texture2dId = assets::AssetHandle<Texture2D>;
	using TextureArrayId = assets::AssetHandle<TextureArray>;
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

	template<>
	struct hash<renderer::FboTextureDescriptor> {
		std::size_t operator()(const renderer::FboTextureDescriptor& s) const noexcept {
			return std::hash<std::string>()(s.name);
		}
	};
	template<>
	struct hash<renderer::TexturePath> {
		std::size_t operator()(const renderer::TexturePath& s) const noexcept {
			std::hash<std::string> hasher;
			return hasher(s.path);
		}
	};
}