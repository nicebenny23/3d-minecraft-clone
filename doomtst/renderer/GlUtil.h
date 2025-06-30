#pragma once
#include <GLFW/glfw3.h>
namespace GlUtil {
	template<typename T>
	constexpr GLenum getGLType();

	template<>
	constexpr GLenum getGLType<float>() { return GL_FLOAT; }

	template<>
	constexpr GLenum getGLType<int>() { return GL_INT; }

	template<>
	constexpr GLenum getGLType<unsigned int>() { return GL_UNSIGNED_INT; }

	template<>
	constexpr GLenum getGLType<double>() { return GL_DOUBLE; }

	template<>
	constexpr GLenum getGLType<short>() { return GL_SHORT; }

	template<>
	constexpr GLenum getGLType<unsigned short>() { return GL_UNSIGNED_SHORT; }

	template<>
	constexpr GLenum getGLType<char>() { return GL_BYTE; }

	template<>
	constexpr GLenum getGLType<unsigned char>() { return GL_UNSIGNED_BYTE; }
	inline size_t Size(GLenum Type){
		// Calculate the size of the attribute in bytes
		switch (Type) {
		case GL_FLOAT:
			return sizeof(float);
			break;
		case GL_INT:
			return sizeof(int);
			break;
		case GL_BYTE:
			return sizeof(char);
			break;
		case GL_UNSIGNED_BYTE:
			return sizeof(unsigned char);
			break;
		case GL_SHORT:
			return sizeof(short);
			break;
		case GL_UNSIGNED_SHORT:
			return sizeof(unsigned short);
			break;
		case GL_DOUBLE:
			return sizeof(double);
			break;
		case GL_UNSIGNED_INT:
			return sizeof(unsigned int);
			break;
			// Add any other types you need to support
		default:
			// Handle unexpected type or assert here
			throw("Unknown type in VertexAttribute");
			break;
		}
		
	}

inline	void set_attr(GLuint index, GLint compnumb, GLenum type, GLsizei stride, size_t offset) {


		//note the empty switch statments are intentional 
		switch (type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			glVertexAttribIPointer(index, compnumb, type, stride, (void*)offset);
			break;
			//for noninteger values
		case GL_FLOAT:
		case GL_DOUBLE:

			glVertexAttribPointer(index, compnumb, type, GL_FALSE, stride, (void*)offset);
			break;
		default:
			throw std::invalid_argument("Cannot set an unsuported type of attribute");
			break;
		}
	}
inline void SetProperty(GLenum Prop, bool On) {
	if (On)
	{
		glEnable(Prop);
	}
	else
	{
		glDisable(Prop);
	}
}
}