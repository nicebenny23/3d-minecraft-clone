#pragma once
#include "VertexObjectManager.h"

namespace VObjMan {

	void VObjManager::Create(buffer_object::Ebo* NewEbo)
	{
		NewEbo->generate();
	}
	void VObjManager::Create(buffer_object::Vbo* NewVbo)
	{
		NewVbo->generate();
	}
	void VObjManager::Create(VaoName::Vao* NewVao)
	{
		NewVao->generate();
	}
	void VObjManager::Destroy(buffer_object::Vbo* Vbo)
	{	
		Vbo->destroy();
	}
	void VObjManager::Destroy(VaoName::Vao* Vao)
	{
		Vao->destroy();
	}
	void VObjManager::Destroy(buffer_object::Ebo* Ebo)
	{

	
			
		
		Ebo->destroy();
	}
}