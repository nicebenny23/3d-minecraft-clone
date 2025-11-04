#pragma once
#include "../../util/dynamicarray.h"
#include <glad/glad.h>
#include "GlUtil.h"
#include "vertex.h"
namespace VObjMan {
	struct VObjManager; // Forward declaration
}
namespace Meshes {
	struct MeshRegistry;

}
namespace VaoName {
	

	struct Vao
	{

		bool IsValid() {
			return id != 0;
		}
		GLuint id;
		vertice::vertex attributes;
		template <typename T, GLint Components>
	
		
		void Clear() {
			id = 0;
			attributes.Clear();
		}
		
		Vao() :attributes(), id(0) {};
		

		void SetAllAttributes() {
			if (!IsValid())
			{
				throw std::logic_error("VAO must be initialized before setting attributes");
			}
			size_t stride =attributes.stride();
			size_t offset = 0;
			for (int i = 0; i < attributes.length(); i++)
			{
				GlUtil::set_attr(i, attributes[i].components, attributes[i].type,GLsizei(stride), offset);
				glEnableVertexAttribArray(i);
				offset += attributes[i].size;
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
			attributes.Clear();
			glDeleteVertexArrays(1, &id);

		}

		friend struct VObjMan::VObjManager;
		void bind() {
			if (!IsValid())
			{
				throw std::logic_error("VAO must be initialized before being bound");
			}
			glBindVertexArray(id);
		}


	};
}

