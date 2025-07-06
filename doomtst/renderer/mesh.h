#include "vertexobject.h"	
#include "Vao.h"
#pragma once
struct Mesh
{
	Mesh() {
		length = -1;
		IsEboMesh = true;
		BuffersGenerated = false;

	}
	bool HasBeenFilled() {

		return (length != -1);
	}
	bool IsEboMesh;
	bool BuffersGenerated;
	int length;
	template <typename T, GLint Components>
	VaoName::Vao& AddAttribute() {
		Voa.AddAttribute<T,Components>();
		return Voa;
	}
	VaoName::Vao Voa;
	buffer_object::Ebo Ibo;
	buffer_object::Vbo Vbo;
		
};

