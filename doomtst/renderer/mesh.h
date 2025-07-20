#include "vertexobject.h"	
#include "Vao.h"
#pragma once
struct Mesh
{
	Mesh() {
		length = -1;
		BuffersGenerated = false;

	}
	bool HasBeenFilled() {

		return (length != -1);
	}

	bool BuffersGenerated;
	int length;
	
	VaoName::Vao Voa;
	buffer_object::Ebo Ibo;
	buffer_object::Vbo Vbo;
		
};

