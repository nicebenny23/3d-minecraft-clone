// deuqe.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#include <assert.h>

#pragma once 
namespace stackname {
	template <typename T>
	struct stacknode
	{

		stacknode<T>* PrevNode;
		T Elem;

		stacknode(const T& val);

	};


	template<typename T>
	class stack {

		stacknode<T>* Top;
	public:
		T pop();
		T& peek();
		const T& peek() const;
		void removetop();
		int length;

		void push(const T& val);

		stack();
		bool  empty() const;
		//no destructor for game engine puropses
		void destroy();

		stack(const stack& other);

		stack& operator=(const stack& other);


		
	};


	template<typename T>
	stack<T>::stack() :length(0), Top(nullptr) {

	}

	template<typename T>
	stacknode<T>::stacknode(const T& val) : Elem(val), PrevNode(nullptr)
	{
	}

	//nonconst version
	template<typename T>
	T& stack<T>::peek()
	{
		if (!empty())
		{
			return Top->Elem;
		}
		throw std::out_of_range("Error: Attempted to peek an empty stack.");
	}
	template<typename T>
	const T& stack<T>::peek() const
	{
		if (!empty())
		{
			return Top->Elem;
		}
		throw std::out_of_range("Error: Attempted to peek an empty stack.");
	}
	template<typename T>
	void stack<T>::removetop()
	{
		if (empty())
		{
			throw std::out_of_range("Error: Attempted to remove the top elmenent of an empty stack.");
		}



		stacknode<T>* cursor = Top;
		Top = Top->PrevNode;
		delete cursor;
		length--;


	}
	template<typename T>
	T stack<T>::pop()
	{
		if (empty())
		{
			throw std::out_of_range("Error: Attempted to pop from an empty stack.");
		}
		T val = peek();
		removetop();
		return val;
	}

	template<typename T>
	void stack<T>::push(const T& val)
	{

		stacknode<T>* newelem = new stacknode<T>(val);

		newelem->PrevNode = Top;
		Top = newelem;
		length++;


	}



	template<typename T>
	bool   stack<T>::empty() const
	{
		return (length == 0);
	}

	template<typename T>
	void stack<T>::destroy()
	{
		while (!empty())
		{
			removetop();
		}
	}

	template<typename T>
	stack<T>& stack<T>::operator=(const stack<T>& other)
	{
		if (this == &other)
		{
			return *this;
		};  // Self-assignment check

		destroy();  // Clean up current stack
		length = other.length;

		if (other.Top == nullptr)
		{
			return *this;
		}
		Top = new stacknode<T>(other.Top->Elem);
		stacknode<T>* iter = other.Top->PrevNode;

		stacknode<T>* bottom = Top;
		while (iter != nullptr)
		{
			bottom->PrevNode = new stacknode<T>(iter->Elem);
			bottom = bottom->PrevNode;

			iter = iter->PrevNode;

		}

		return *this;


	}

	template<typename T>
	stack<T>::stack(const stack& other) :length(0), Top(nullptr) {
		*this = other;
	}


}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
