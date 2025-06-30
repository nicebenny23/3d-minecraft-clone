#pragma once
#include "../util/dynamicarray.h"
#include <glad/glad.h>
#include "GlUtil.h"

namespace VObjMan {
	class VObjManager; // Forward declaration
}
namespace VaoName {
	struct VertexAttribute {
		
		GLint Components; // Number of components (e.g., 3 for a vec3)
		GLenum Type;     // The OpenGL type (e.g., GL_FLOAT, GL_INT)
		VertexAttribute()
			: Components(0), Type(GL_NONE), size(0) {}

		// Template constructor
	
		VertexAttribute(GLenum type, GLint Comps) {
			size = Comps * GlUtil::Size(type);
			Components = Comps;
			Type = type;
		}

		// Calculate the size of the attribute in bytes
		size_t Size() const {

			return size;

		}
	private: 
		size_t 	size;
	};

	struct Vao
	{

		bool IsValid() {
			return id != 0;
		}
		GLuint id;
		Cont::array<VertexAttribute> attributes;
		template <typename T, GLint Components>
		Vao& AddAttribute( ) {
			attributes.push(VertexAttribute::VertexAttribute(GlUtil::getGLType<T>(), Components));
			return *this;
		}
		Vao& AddAttribute(GLenum Type,GLint Components) {
			attributes.push(VertexAttribute(Type,Components));
			return *this;
		}
		void ClearAttributes() {

			attributes.destroy();
		}
		void removeAttribute(size_t index) {
			if ( attributes.length<= index ) {
				throw std::logic_error("Unable to remove invalid attribute");
			}
			attributes.deleteind(index);

		}
		Vao() {

			id = 0;
		}
		int ComponentsPerVertex() {
			int Comps= 0;
			for (int i = 0; i < attributes.length; i++) {


				Comps+= attributes[i].Components;  // Sum up the size for stride calculation

			}
			return Comps;
		}
		int GetStride() {
			int stride = 0;
			for (int i = 0; i < attributes.length; i++) {


				stride += attributes[i].Size();  // Sum up the size for stride calculation

			}
			return stride;
		}

		void SetAllAttributes() {
			if (!IsValid())
			{
				throw std::logic_error("VAO must be initialized before setting attributes");
			}
			size_t stride = GetStride();
			size_t offset = 0;
			for (int i = 0; i < attributes.length; i++)
			{

			GlUtil::set_attr(i, attributes[i].Components, attributes[i].Type, stride, offset);
				glEnableVertexAttribArray(i);
				offset += attributes[i].Size();


			}

		}
	private:
		void generate() {
			if (IsValid())
			{
				throw std::logic_error("Cannot Initilize Already Initlized Vao");
			}
			glGenVertexArrays(1, &id);

		}
		void destroy() {
			if (!IsValid())
			{
				throw std::logic_error("Cannot Destroy Invalid Vao");
			}
			attributes.destroy();
			glDeleteVertexArrays(1, &id);

		}

		friend class VObjMan::VObjManager;
		void bind() {
			if (!IsValid())
			{
				throw std::logic_error("VAO must be initialized before being bound");
			}
			glBindVertexArray(id);
		}


	};
}

