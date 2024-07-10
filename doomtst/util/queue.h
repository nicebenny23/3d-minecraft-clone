#pragma once
namespace queuename {
	template <typename T>
	struct ptrandelem
	{

		ptrandelem<T>* ptrnext;
		T elem;

		ptrandelem(T val);
		ptrandelem(T val, ptrandelem<T>* nextelem);
	};


	template<typename T>
	class queue {

		ptrandelem<T>* top;
		ptrandelem<T>* bottom;
	public:
		bool swaptop(T val);
		T pop();
		T peek();
		bool remove();
		int length;
		void append(T val);
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
	ptrandelem<T>::ptrandelem(T val)
	{

		elem = T(val);
		ptrnext = this;
	}

	template<typename T>
	ptrandelem<T>::ptrandelem(T val, ptrandelem<T>* nextelem)
	{

		ptrnext = nextelem;
		elem = val;
	}

	template<typename T>
	bool queue<T>::swaptop(T val)
	{
		if (!empty())
		{
			top->elem = val;
			return true;
		}
		return false;
	}

	template<typename T>
	T queue<T>::pop()
	{
		if (length == 1)
		{
			ptrandelem<T>* cursor = bottom;
			T val = bottom->elem;

			top = nullptr;
			bottom = nullptr;
			delete cursor;
			length--;
			return val;
		}
		if (!empty())
		{
			ptrandelem<T>* cursor = bottom;
			T val = bottom->elem;

			bottom = bottom->ptrnext;
			delete cursor;
			length--;
			return val;
		}

		return T();

	}

	template<typename T>
	T queue<T>::peek()
	{
		if (!empty())
		{
			return bottom->elem;
		}
		return T();
	}

	template<typename T>
	bool queue<T>::remove()
	{
		if (length == 1)
		{
			ptrandelem<T>* cursor = bottom;



			top = nullptr;
			bottom = nullptr;
			delete cursor;
			length--;
			return true;
		}
		if (!empty())
		{
			ptrandelem<T>* cursor = bottom;


			bottom = bottom->ptrnext;



			delete cursor;
			length--;
			return true;
		}

		return false;
	}

	template<typename T>
	void queue<T>::append(T val)
	{

		if (!empty())
		{

			ptrandelem<T>* newelem = new ptrandelem<T>(val);
			top->ptrnext = newelem;
			top = newelem;

			length++;
			return;
		}
		ptrandelem<T>* newelem = new ptrandelem<T>(val);
		top = newelem;
		bottom = newelem;
		++length;


	}

	template<typename T>
	bool queue<T>::empty()
	{
		if (length == 0) {

			return true;

		}
		return false;
	}



}
