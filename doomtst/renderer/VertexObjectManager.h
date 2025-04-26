#include "Vao.h"
#include "vertexobject.h"
#pragma once
namespace VObjMan {
	class VObjManager
	{
	public:
		VaoName::Vao* BoundVao;
		vobj::Vbo* BoundVbo;
		vobj::Ebo* BoundEbo;
		void Bind(vobj::Ebo* ToBind);
		void Bind(VaoName::Vao* ToBind);
		void Bind(vobj::Vbo* ToBind);
		void Bind(vobj::Ebo& ToBind);
		void Bind(VaoName::Vao& ToBind);
		void Bind(vobj::Vbo& ToBind);
		void Create(vobj::Ebo* NewEbo);
		void Create(vobj::Vbo* NewVbo);
		void Create(VaoName::Vao* NewVao);
		void Destroy(vobj::Vbo* Vbo);
		void Destroy(VaoName::Vao* Vao);
		void Destroy(vobj::Ebo* Ebo);
		void UnbindVao();
		void UnbindVbo();
		void UnbindEbo();
		bool isBound(vobj::Vbo* Vbo);
		bool isBound(VaoName::Vao* Vao);
		bool isBound(vobj::Ebo* Ebo);

 	};

}