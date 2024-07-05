#ifndef vertexobj_hpp
#define vertexobj_hpp
#include "dynamicarray.h"

#include <glad/glad.h>
namespace vobj {
	struct vao
	{
		GLuint id;


		vao() {
			id = 0;
		}
		void generate() {
			glGenVertexArrays(1, &id);

		}
		void bind() {

			glBindVertexArray(id);
		}
		//this line copied from internet
		void set_attr(GLuint index, GLint compnumb, GLenum type, GLsizei stride, size_t offset) {

			//size is total size
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
			default:
				glVertexAttribPointer(index, compnumb, type, GL_FALSE, stride, (void*)offset);
				break;
			}
		}

	
		
	};
	struct vbuf
	{
		GLuint id;
		GLuint type;
		
		vbuf(const GLuint& ID, const GLuint& TYPE){

			id = ID;
		type = TYPE;
		}

		vbuf() {

			id = 0;
			type = 0;
			
		}
		void generate(GLuint buftype) {
			glGenBuffers(1, &id);
			type = buftype;
		}
		void bind() {
			if (type != 0)
			{
				glBindBuffer(type, id);
			}

		}
	


		//use when data is of one type
		template<class T>
		void fillbuffer(dynamicarray::array<T>& data) {


			glBufferData(type, sizeof(T)*data.length, data.getdata(), GL_STATIC_DRAW);
		}
		//fills data unnasotided
		void fillbuffer(void* data,int size) {


			glBufferData(type, size, data, GL_STATIC_DRAW);

		}
		

		
	};

}


#endif // !vertexobj_H
