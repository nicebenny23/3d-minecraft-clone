
#include <stdexcept>
#pragma once
namespace queuename {
	template <typename T>
	struct queuenode
	{

		queuenode<T>* ptrnext;
		T elem;

		queuenode(const T& val);
	
	};


	template<typename T>
	class queue {

		queuenode<T>* Back;
		queuenode<T>* Front;
	
	public:

		
		T pop();
		T& peek();
		const T& peek() const;
		void destroy();
		void removefront();
		int length;
		void push(const T& val);
		void push( T&& val);
		queue();
		bool empty() const;
		queue(const queue<T>& other);
		queue<T>& operator=(const queue<T>& other);
		queue(queue<T>&& other) noexcept;
		queue<T>& operator=(queue<T>&& other) noexcept;
		~queue() {
			destroy();
		}
	};
	template<typename T>
	queuenode<T>::queuenode(const T& val) :elem(val), ptrnext(nullptr)
	{
	}

	template<typename T>
	queue<T>::queue() {
		length = 0;
		Back = nullptr;
		Front = nullptr;


	}

	

	template<typename T>
	bool queue<T>::empty() const
	{
		return  (length == 0);
	}

	template<typename T>
	T queue<T>::pop()
	{
		
		if (!empty())
		{
			queuenode<T>* cursor = Front;
			T val = Front->elem;
			removefront();
			return val;
		}


		throw std::out_of_range("Error: Attempted to pop from an empty queue.");

	}

	template<typename T>
	T& queue<T>::peek()
	{
		if (!empty())
		{
			return Front->elem;
		}
		throw std::out_of_range("Error: Attempted to peek from an empty queue.");
	}

	template<typename T>
	const T& queue<T>::peek() const
	{
		 if (!empty())
		 {
			 return Front->elem;
		 }
		 throw std::out_of_range("Error: Attempted to peek from an empty queue.");
	}

	 template<typename T>
	  void queue<T>::destroy()
	 {
		 while (!empty())
		 {
			 removefront();
		 }
	 }

	template<typename T>
	void queue<T>::removefront()
	{
		if (empty())
		{
			throw std::out_of_range("Error: Attempted to remove the top element of an empty queue.");
		}
		queuenode<T>* cursor = Front;
		if (length == 1)
		{
			Back = nullptr;
			Front = nullptr;
		}
		else
		{
			Front = Front->ptrnext;
		}
		delete cursor;
		length--;
		
	
	}

	template<typename T>
	void queue<T>::push(const T& val)
	{

		queuenode<T>* newelem = new queuenode<T>(val);
		

		if (empty())
		{
			//since it is the first element it goes on bottom
			Front = newelem;
		}
		else
		{
			//
			Back->ptrnext = newelem;
		}
		Back = newelem; 
		length++;
	


	}

	template<typename T>
	 void queue<T>::push( T&& val)
	{
		queuenode<T>* newelem = new queuenode<T>(std::move(val));


		if (empty())
		{
			//since it is the first element it goes on bottom
			Front = newelem;
		}
		else
		{
			//
			Back->ptrnext = newelem;
		}
		Back = newelem;
		length++;

	}

	

	template<typename T>
	 queue<T>::queue(const queue<T>& other) : Front(nullptr), Back(nullptr), length(0)
	{
		queuenode<T>* cursor= other.Front;
		while (cursor)
		{
			push(cursor->elem);
			cursor = cursor->ptrnext;
		}
	}

	template<typename T>
	 queue<T>& queue<T>::operator=(const queue<T>& other)
	{
		if (this == &other) {

			return *this;
		}
		destroy();
		queuenode<T>* cursor = other.Front;
		while (cursor)
		{
			push(cursor->elem);
			cursor = cursor->ptrnext;
		}

		return *this;
	}

	 template<typename T>
	  queue<T>::queue(queue<T>&& other) noexcept :Back(other.Back),Front(other.Front),length(other.length)
	 {
		 other.Front = nullptr;
		 other.Back = nullptr;
		 other.length = 0;
	 }

	 template<typename T>
	  queue<T>& queue<T>::operator=(queue<T>&& other) noexcept
	 {

		 if (this==&other)
		 {
			 return *this;
		 }
		 destroy();
		 Back = other.Back;
		 Front = other.Front;
		 length = other.length;
		 other.length = 0;
		 other.Front = nullptr;
		 other.Back = nullptr;
		 return *this;
		 // TODO: insert return statement here
	 }


}
