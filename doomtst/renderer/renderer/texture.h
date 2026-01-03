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
enum TextureType {
	TexArray = GL_TEXTURE_2D_ARRAY,
	Tex2d = GL_TEXTURE_2D,
	Tex3d = 2,
	TextureNull = 3,
}; namespace TextureManager {
	class TextureManager; // Forward declaration
}

struct Texture2D:assets::Asset
{
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

namespace std {
	template<>
	struct hash<TexturePath> {
		std::size_t operator()(const TexturePath& s) const noexcept {
			std::hash<std::string> hasher;
			return hasher(s.name) + hasher(s.path);
		}
	};
}

inline void settextureparams(GLint texturetype) {

	glGenerateMipmap(texturetype);
	glTexParameteri(texturetype, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(texturetype, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

}
struct TextureLoader {
	TextureLoader() {

	}
	using load_descriptor = TexturePath;
	stn::box<Texture2D> load(const TexturePath& path) {
		v2::Coord2 size;
		unsigned char* data = texdata::loadtexdata(&size.x, &size.y, path.path.c_str());
		GLuint id=0;
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
struct TextureArray:assets::Asset
{
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
	void destroy(){
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
	bool operator==(const TextureArrayPath& other) const{
		return name == other.name && paths == other.paths;
	}
	TextureArrayPath(const stn::array<std::string>& paths, const std::string& name) :paths(paths), name(name) {
	}
};

namespace std {
	template<>
	struct hash<TextureArrayPath> {
		std::size_t operator()(const TextureArrayPath& s) const noexcept {
			std::hash<std::string> hasher;
			std::hash<stn::array<std::string>> path_hasher;
			return hasher(s.name) + path_hasher(s.paths);
		}
	};
}


struct TextureArrayLoader {
	TextureArrayLoader() {

	}
	using load_descriptor = TextureArrayPath;
	stn::box<TextureArray> load(const TextureArrayPath& path) {

		GLuint id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D_ARRAY, id); 
		v2::Coord2 size;
		if (path.count()!= 0) {
			size = texdata::GetImgSize(path.paths[0].c_str());
		}


		glTexImage3D(
			GL_TEXTURE_2D_ARRAY //type
			, 0                 //minmap level
			, GL_RGBA8            //color format
			, size.x, size.y, path.count()//size
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
