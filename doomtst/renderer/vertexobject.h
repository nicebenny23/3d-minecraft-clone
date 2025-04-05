#ifndef vertexobj_hpp
#define vertexobj_hpp
#include "../util/dynamicarray.h"

#include <glad/glad.h>
inline GLint getcurvoa(){
	GLint vaoID;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &vaoID);
	return vaoID;
}
namespace vobj {
	struct vao
	{
		GLuint id;


		vao() {
			id =0;
		}
		void generate() {
			glGenVertexArrays(1, &id);

		}
		void destroy() {
			
			glDeleteVertexArrays(1, &id);

		}

		void bind() {

			glBindVertexArray(id);
		}
		//this line copied from internet
		void set_attr(GLuint index, GLint compnumb, GLenum type, GLsizei stride, size_t offset) {

			
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
				Assert("attempted to set unsuported type of attribute");
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
	
		void destroy() {

			glDeleteBuffers(1, &id);

		}


	
		template<class T>
		void tfillbuffer(dynamicarray::array<T>& data) {
			GLint prevID; // Variable to store the previously bound buffer ID
			GLint buffertype; // Variable to store the buffer type (array buffer or element array buffer)
			// Determine the buffer type (GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
			switch (type) {
			case GL_ARRAY_BUFFER:
				buffertype = GL_ARRAY_BUFFER_BINDING;  // Set the binding type to GL_ARRAY_BUFFER_BINDING
				break;
			case GL_ELEMENT_ARRAY_BUFFER:
				buffertype = GL_ELEMENT_ARRAY_BUFFER_BINDING;  // Set the binding type to GL_ELEMENT_ARRAY_BUFFER_BINDING
				break;
			default:
				Assert("unsupported buffertype");  // Error handling for unsupported buffer types
				break;
			}
			// Get the current bound buffer ID for the specified buffer type
			glGetIntegerv(buffertype, &prevID);  // Retrieve the ID of the currently bound buffer (GL_ARRAY_BUFFER or GL_ELEMENT_ARRAY_BUFFER)
			// Bind this buffer to the current buffer type
			bind();  // Binds the current buffer (this function is defined elsewhere in the code)

			// Fill the buffer with the new data from the dynamic array
			fillbuffer(data.list, sizeof(T) * data.length);  // Fills the buffer with data from the array

			// Restore the previous buffer binding to prevent any side-effects
			glBindBuffer(type, prevID);  // Bind the previous buffer ID back to its original state
		}

		//fills data unnasotided
		void fillbuffer(void* data,int size) {
			glBufferData(type, size, data, GL_STATIC_DRAW);

		}
			//use when data is of one type
		template<class T>
		void fillbuffer(dynamicarray::array<T>& data) {
			fillbuffer(data.list, data.length*sizeof(T));
		}

		
	};

}


#endif // !vertexobj_H
