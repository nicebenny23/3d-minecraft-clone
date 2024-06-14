#include "vector3.h"
#include "dynamicarray.h"

#include <unordered_map>
#pragma once
using namespace v3;

using namespace dynamicarray;
#ifndef gameobj_HPP


#define gameobj_HPP
namespace gameobject {




	extern std::unordered_map<char*, int> stringtoint;
	extern int curid;
	inline void initmap() {



		stringtoint = std::unordered_map<char*, int>();
		curid = 0;
	}

	inline	int idfromnameadd(char* name) {
		if (stringtoint.count(name) == 0)
		{

			stringtoint.emplace(name, curid);
			curid++;
		}

		return stringtoint[name];
	}


	inline int idfromname(char* name) {
		if (stringtoint.count(name) == 0)
		{

			return -1;
		}

		return stringtoint[name];
	}





	struct probguid
	{
		int guid1;
		int guidcheck;
	};


	struct gameobj;

	struct component
	{
		//called on destroy used for deallocation
		virtual void ondestroy();
		gameobj* holder;
		void setobj(gameobj* obj) {
			holder = obj;
		}
		component() {


		};
		virtual component* copydat(component* orgin);
		virtual void start();

		virtual void update();



		int id;

	};



	struct gameobj
	{
		int guid;
		bool tsty;
		array<component*> complist;

		gameobj(Vector3 ipos, const char* _name);
		Vector3 pos;
	
		const	char* name;


		template <class T>
		T& getcomponent();

		template <class T>
		bool hascomponent();

		template <class T>
		void removecomponent();

		template <class T>
		array<T>  getcomponents();


		template <class T, typename... types>
		void addcomponent(types&&... initval);


		gameobj();

	};


	void destroy(gameobj* obj);
	void deleteobjs();
	void initobjs();
	void runupdateloop();
	template <class T>
	void gameobj::removecomponent()
	{
		int id = idfromname((char*)(typeid(T).name()));
		if (id == -1)
		{
			return;
		}
		for (int i = 0; i < complist.length; i++)
		{
			if (id = complist[i]->id) {
				complist.deleteind(i);

				return;
			}
		}
	}



	template <class T>
	T& gameobj::getcomponent()
	{


		int id = idfromname((char*)(typeid(T).name()));
		if (id == -1)
		{

		}
		for (int i = 0; i < complist.length; i++)
		{

			if (id == complist[i]->id) {


				return *((T*)complist[i]);
			}
		}
	}

	template <class T>
	array<T> gameobj::getcomponents()
	{




		int id = idfromnameadd((char*)(typeid(T).name()));
		if (id == -1)
		{

		}
		array<T> comps = *(new array<T>());
		for (int i = 0; i < complist.length; i++)
		{

			if (id == complist[i]->id) {
				comps.append(*dynamic_cast<T*>(complist[i]));
			}
		}

		return comps;

	}


	template <class T>
	bool gameobj::hascomponent()
	{


		int id = idfromname((char*)(typeid(T).name()));
		if (id == -1)
		{

		}
		for (int i = 0; i < complist.length; i++)
		{

			if (id == complist[i]->id) {


				return true;
			}
		}
		return false;
	}





	template <class T, typename... types>


	void gameobj::addcomponent(types&&... initval)
	{





		int id = idfromnameadd((char*)(typeid(T).name()));
		static_assert(std::is_constructible_v<T, types...>, "no constructer takes these parameters");
		static_assert(std::is_base_of<component, T>::value, "T is not a component");

		T* comp = new T(std::forward<types>(initval)...);

		comp->setobj(this);

		comp->start();

		comp->id = idfromname((char*)(typeid(T).name()));
		for (int i = 0; i < complist.length; i++)
		{
			if (id > complist[i]->id) {

				complist.insertind(i, comp);
				return;
			}
		}
		complist.append(comp);

	}
	//is a guid with 2 numbers one for hashing and another for checking this basicly ellimiantes any prossiblity for collision as the other one can go to 2billion


	struct gameobjref
	{
		int guid;

		gameobjref(gameobj& obj) {
			guid = obj.guid;


		}
		gameobj* obj();
		gameobjref() {

			int guid = 0;
		}
	};




	gameobjref gameinit(Vector3 ipos, const char* _name);


}
#endif#pragma once
