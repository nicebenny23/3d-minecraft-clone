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

		queuenode<T>* top;
		queuenode<T>* bottom;
	public:
		
		T pop();
		T& peek();
		T& peek() const;
		void destroy();
		void removefront();
		int length;
		void push(const T& val);
		queue();
		bool empty();



	};


	template<typename T>
	queue<T>::queue() {
		length = 0;
		top = nullptr;
		bottom = nullptr;


	}

	template<typename T>
	queuenode<T>::queuenode(const T& val)
	{

		elem = T(val);
		ptrnext = nullptr;
	}

	

	template<typename T>
	T queue<T>::pop()
	{
		
		if (!empty())
		{
			queuenode<T>* cursor = bottom;
			T val = bottom->elem;
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
			return bottom->elem;
		}
		throw std::out_of_range("Error: Attempted to peek from an empty queue.");
	}

	template<typename T>
	 T& queue<T>::peek() const
	{
		 if (!empty())
		 {
			 return bottom->elem;
		 }
		 throw std::out_of_range("Error: Attempted to peek from an empty queue.");
	}

	 template<typename T>
	 inline void queue<T>::destroy()
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
		queuenode<T>* cursor = bottom;
		if (length == 1)
		{
			top = nullptr;
			bottom = nullptr;
		}
		else
		{
			bottom = bottom->ptrnext;
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
			bottom = newelem;
		}
		else
		{
			//
			top->ptrnext = newelem;
		}
		top = newelem; 
		length++;
		return;


	}

	template<typename T>
	bool queue<T>::empty()
	{
		return  (length == 0);
	}


}
