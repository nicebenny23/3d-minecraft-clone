#include "vertexobject.h"	
#include "Vao.h"
#pragma once
struct Mesh
{
	Mesh() {
		length = 0;
		BuffersGenerated = false;

	}
	bool filled() const {

		return (length != 0)&& generated();
	}
	bool generated() const {

		return BuffersGenerated;
	}

	bool BuffersGenerated;
	size_t length;
	VaoName::Vao Voa;
	buffer_object::Ebo Ibo;
	buffer_object::Vbo Vbo;
	GLuint vao_id() const{
		return Voa.id;
	}
	GLuint ebo_id() const {
		return Ibo.Buf.id;
	}
	GLuint vbo_id() const {
		return Vbo.Buf.id;
	}
};



