#pragma once
#include "../../util/dynamicarray.h"

#include <glad/glad.h>
namespace VObjMan {
	struct VObjManager; // Forward declaration
}
namespace buffer_object {
	
	struct BufferObj
	{
		GLuint id;

		
		BufferObj(GLint Type) {
			id = -1;
			type = Type;
			
		}
	

	
		//fills data unnasotided
		void fillbuffer(void* data,size_t size) {
			if (data!=nullptr)
			{
				glBufferData(type, size, data, GL_STATIC_DRAW);
			}

		}
			//use when data is of one type
		template<class T>
		void fillbuffer(stn::array<T>& data) {
			fillbuffer(data.data(), data.length() * sizeof(T));
		}
	protected:
		void generate() {
			glGenBuffers(1, &id);

		}

		void destroy() {

			if (id == -1)
			{
				throw std::logic_error("Alreadly deleted Buffer");
			}
			glDeleteBuffers(1, &id);
			id = -1;
		}

		GLenum type;
		friend struct Ebo;
		friend struct Vbo;
		friend struct VObjMan::VObjManager;
		void bind() {
			if (type != 0)
			{
				glBindBuffer(type, id);
			}

		}

	};



	struct Ebo
	{
		BufferObj Buf;
		Ebo():Buf(GL_ELEMENT_ARRAY_BUFFER) {	
		}
		template<class T>
		void fillbuffer(stn::array<T>& data) {
			Buf.fillbuffer<T>(data);
		}
		void fillbuffer(void* data, int size) {
			Buf.fillbuffer(data, size);
		}
		GLint id() {
			return Buf.id;
		}
	private:
		void generate() {
			Buf.generate();
		}
		void destroy() {
			Buf.destroy();
		}
		void Bind() {
			Buf.bind();
		}
		friend struct VObjMan::VObjManager;
	};




	struct Vbo
	{
		BufferObj Buf;
		Vbo() :Buf(GL_ARRAY_BUFFER) {
		}
		template<class T>
		void fillbuffer(stn::array<T>& data) {
			Buf.fillbuffer<T>(data);
		}
		void fillbuffer(void* data, int size) {
			Buf.fillbuffer(data, size);
		}
		GLint id() const {
			return Buf.id;
		}
	private:
		void generate() {
			Buf.generate();
		}
		void destroy() {
			Buf.destroy();
		}

		void Bind() {
			Buf.bind();

		}
		friend struct VObjMan::VObjManager;
	};
}

