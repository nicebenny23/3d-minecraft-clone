#include "Vao.h"
#include "vertexobject.h"
#pragma once
namespace VObjMan {
	class VObjManager
	{
	public:
		void Create(buffer_object::Ebo* NewEbo);
		void Create(buffer_object::Vbo* NewVbo);
		void Create(VaoName::Vao* NewVao);
		void Destroy(buffer_object::Vbo* Vbo);
		void Destroy(VaoName::Vao* Vao);
		void Destroy(buffer_object::Ebo* Ebo);
	
 	};

}