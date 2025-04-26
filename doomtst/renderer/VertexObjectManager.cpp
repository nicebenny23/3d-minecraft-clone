#include "VertexObjectManager.h"

namespace VObjMan {
	void VObjManager::Bind(vobj::Ebo* ToBind)
	{

		if (ToBind == nullptr)
		{
			throw std::invalid_argument("Cannot bind null ptr");
		}

		if (isBound(ToBind))
		{
			return;
		}
		ToBind->Bind();
		BoundEbo = ToBind;
	}

	void VObjManager::Bind(VaoName::Vao* ToBind)
	{

		if (ToBind == nullptr)
		{
			throw std::invalid_argument("Cannot bind null ptr");
		}

		if (isBound(ToBind))
		{
			return;
		}
		ToBind->bind();
		BoundVao = ToBind;
	}

	void VObjManager::Bind(vobj::Vbo* ToBind)
	{
		if (ToBind == nullptr)
		{
			throw std::invalid_argument("Cannot bind null ptr");
		}

		if (isBound(ToBind))
		{
			return;
		}
		ToBind->Bind();
		BoundVbo = ToBind;
	}
	void VObjManager::Bind(vobj::Ebo& ToBind)
	{
		Bind(&ToBind);
	}
	void VObjManager::Bind(VaoName::Vao& ToBind)
	{

		Bind(&ToBind);
	}
	void VObjManager::Bind(vobj::Vbo& ToBind)
	{

		Bind(&ToBind);
	}
	void VObjManager::Create(vobj::Ebo* NewEbo)
	{
		NewEbo->generate();
	}
	void VObjManager::Create(vobj::Vbo* NewVbo)
	{
		NewVbo->generate();
	}
	void VObjManager::Create(VaoName::Vao* NewVao)
	{
		NewVao->generate();

	}
	void VObjManager::Destroy(vobj::Vbo* Vbo)
	{

		if (isBound(Vbo))
		{
			UnbindVbo();
		}
		Vbo->destroy();
	}
	void VObjManager::Destroy(VaoName::Vao* Vao)
	{

		if (isBound(Vao))
		{
			UnbindVao();
		}
		Vao->destroy();
	}
	void VObjManager::Destroy(vobj::Ebo* Ebo)
	{

		if (isBound(Ebo))
		{
			UnbindEbo();
		}
		Ebo->destroy();
	}
	void VObjManager::UnbindVao()
	{

		glBindVertexArray(0);
		BoundVao = nullptr;
	}
	void VObjManager::UnbindVbo()
	{
		BoundVbo = nullptr;
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	void VObjManager::UnbindEbo()
	{
		BoundEbo = nullptr;
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	bool VObjManager::isBound(vobj::Vbo* Vbo)
	{
		if (Vbo == nullptr || BoundVbo == nullptr)
		{
			return false;
		}
		return BoundVbo->Getid() == Vbo->Getid();

	}
	bool VObjManager::isBound(VaoName::Vao* Vao)
	{
		if (Vao == nullptr || BoundVao == nullptr)
		{
			return false;
		}

		return BoundVao->id == Vao->id;
	}
	bool VObjManager::isBound(vobj::Ebo* Ebo)
	{
		if (Ebo == nullptr || BoundEbo == nullptr)
		{
			return false;
		}
		return BoundEbo->Getid() == Ebo->Getid();

	}
}