#include "vector3.h"
#include "dynamicarray.h"

#include <unordered_map>
#pragma once


using namespace dynamicarray;
#ifndef gameobj_HPP

#define gameobj_HPP
#define entsize 16*16*16*250
namespace gameobject {



	//these 3 function are used to get a unique component number f
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

	
	inline int compidfromname(char* name) {
		if (stringtoint.count(name) == 0)
		{
			_STATIC_ASSERT("type in name of componenent");
			return -1;
		}

		
		return stringtoint[name];
	}









	
	struct obj;
	
	
	extern array<obj*> objectfromguid;

	struct component
	{
		//called on destroy used for deallocation
		virtual void ondestroy();
		obj* owner;
		virtual ~component() = default;
		virtual void setobj(obj* object) {
			owner = object;
		}
		component() {
			owner = nullptr;
			id = -1;
		};
		virtual component* copydat(component* orgin);
		virtual void start();

		virtual void update();
		virtual void blockfaceupdate(obj* blk,int face);


		int id;

	};
	//determanes the type of object a give object is,
	enum objtype {
		block = 0,
		entity = 1
		
	};
	struct obj
	{
		int guid;

		array<component*> complist;

		obj(v3::Vector3 ipos, const char* _name);


		objtype type;

		template <class T>
		T& getcomponent();

		template <class T>
		bool hascomponent();
		//removes a component
		template <class T>
		void removecomponent();

		template <class T>
		array<T>  getcomponents();


		template <class T, typename... types>
		void addcomponent(types&&... initval);

		//todo -mid proiorty implement;
		void blkfaceupdate(obj* blk, int face);

		obj();

	};


	void destroy(obj* object);
	void deleteobjs();
	int getgoid();
	void initobjs();
	void runupdateloop();

	template <class T>
	void obj::removecomponent()
	{
		int id = compidfromname((char*)(typeid(T).name()));
		if (id == -1)
		{
			return;
		}
		for (int i = 0; i < complist.length; i++)
		{
			int l = complist[i]->id;
			if (id == complist[i]->id) {
				complist.deleteind(i);
				i--;
				
			}
		}
		return;
	}



	template <class T>
	T& obj::getcomponent()
	{


		int id = compidfromname((char*)(typeid(T).name()));
		if (id == -1)
		{
			Assert("trying to get undefined component");
		}
		for (int i = 0; i < complist.length; i++)
		{

			if (id == complist[i]->id) {


				return *((T*)complist[i]);
			}
		}
		static_assert("", "");
	}
	//barely used
	template <class T>
	array<T> obj::getcomponents()
	{




		int id = idfromnameadd((char*)(typeid(T).name()));
		if (id == -1)
		{

		}
		array<T> componentlist = *(new array<T>());
		for (int i = 0; i < complist.length; i++)
		{

			if (id == complist[i]->id) {
				componentlist.append(*dynamic_cast<T*>(complist[i]));
			}
		}

		return componentlist;

	}


	template <class T>
	bool obj::hascomponent()
	{


		int id = compidfromname((char*)(typeid(T).name()));
		if (id == -1)
		{
			Assert("component not defined yet");

			return false;
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


	void obj::addcomponent(types&&... initval)
	{





		int id = idfromnameadd((char*)(typeid(T).name()));
	//fix
		//Assert(std::is_constructible_v<T, types...>, "no constructer takes these parameters");
	//	Assert(std::is_base_of<component, T>::value, "T is not a component");

		T* comp = new T(std::forward<types>(initval)...);

		comp->setobj(this);

		comp->start();

		comp->id = compidfromname((char*)(typeid(T).name()));
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


	struct objref
	{
		int guid;
		int probguid;
		objref(obj& object) {
			guid = object.guid;
		}
		obj* toobj();
		objref() {

			 guid = -1;
		}
	};




	inline void immidiatedestroy(obj* object) {

		for (int i = 0; i < object->complist.length; i++)
		{
			object->complist[i]->ondestroy();
		//deletes component refered to by pointer
			delete object->complist[i];

		}
		//deletes pointer itsekf
		object->complist.destroy();
		//makes it so "object from guid is now freed"
		objectfromguid[object->guid] = nullptr;
		
	}

	
	}
#endif#pragma once
