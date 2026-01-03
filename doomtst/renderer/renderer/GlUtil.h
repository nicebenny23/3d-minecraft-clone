#pragma once
#include "../../util/exception.h"
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
	inline size_t Size(GLenum Type) {
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
			stn::throw_logic_error("Unknown type in VertexAttribute with id {}",Type);
			break;
		}

	}

	inline	void set_attr(size_t index, size_t compnumb, GLenum type, size_t stride, size_t offset) {


		//note the empty switch statments are intentional 
		switch (type) {
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
		case GL_SHORT:
		case GL_UNSIGNED_SHORT:
		case GL_INT:
		case GL_UNSIGNED_INT:
			glVertexAttribIPointer(static_cast<GLuint>(index), static_cast<GLsizei>(compnumb), type, static_cast<GLsizei>(stride), (void*)offset);
			break;
			//for noninteger values
		case GL_FLOAT:
		case GL_DOUBLE:

			glVertexAttribPointer(static_cast<GLuint>(index), static_cast<GLsizei>(compnumb), type, GL_FALSE, static_cast<GLsizei>(stride), (void*)offset);
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
	inline void poll_errors() {
		GLint err = glGetError();
		if (err != GL_NO_ERROR) {
			stn::throw_logic_error("open gl error {}", err);
		}
	}
	inline void shaderstatuscheck(GLuint id, GLenum statustype, const char* name) {
		GLint success = 0;

		// Use program calls for LINK and VALIDATE status, shader calls for COMPILE status
		if (statustype == GL_LINK_STATUS || statustype == GL_VALIDATE_STATUS) {
			glGetProgramiv(id, statustype, &success);
		}
		else if (statustype == GL_COMPILE_STATUS) {
			glGetShaderiv(id, statustype, &success);
		}
		else {
			throw std::logic_error("Unsupported statustype passed to shaderstatuscheck");
		}


		GLint logLength = 0;
		if (statustype == GL_LINK_STATUS || statustype == GL_VALIDATE_STATUS) {
			glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
		}
		else { // compile status
			glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
		}

		if (logLength > 0) {
			std::vector<char> infoLog(logLength);
			if (statustype == GL_LINK_STATUS || statustype == GL_VALIDATE_STATUS) {
				glGetProgramInfoLog(id, logLength, nullptr, infoLog.data());
			}
			else { // compile status
				glGetShaderInfoLog(id, logLength, nullptr, infoLog.data());
			}

			infoLog.back() = '\0'; // null terminate safely

			std::string errorType;
			switch (statustype) {
			case GL_COMPILE_STATUS: errorType = "Compile error"; break;
			case GL_LINK_STATUS:    errorType = "Link error"; break;
			case GL_VALIDATE_STATUS:errorType = "Validation error"; break;
			default:                errorType = "Unknown error"; break;
			}

			std::string errorMsg = std::string(name) + " " + errorType + ": " + infoLog.data();
			if (!success) {
				throw std::logic_error(errorMsg);
			}
		}
		else if (!success) {
			{
				throw std::logic_error(std::string(name) + " error: Unknown error (empty info log)");
			}
		}

	}
}