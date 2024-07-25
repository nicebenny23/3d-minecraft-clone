#include "../util/vector3.h"
#include "../util/dynamicarray.h"

#include <unordered_map>
#pragma once


using namespace dynamicarray;
#ifndef gameobj_HPP

#define gameobj_HPP

namespace gameobject {

	enum objstate
	{
		beinginitiated=0,
		active=1,
		
		//deleting components but keeping object
		beingsoftdestroyed=2,
		//deleting object and components
		beingroughdestroyed=3
	};

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
		int priority ;
		//called on destroy used for deallocation
		virtual void ondestroy();
		obj* owner;
		virtual ~component() = default;
		virtual void setobj(obj* object) {
			owner = object;
		}
		bool active;
		component() {
			active = true;
			owner = nullptr;
			id = -1;
		};
		virtual component* copydat(component* orgin);
		virtual void start();
		virtual void renderupdate();
		virtual void update();
		virtual void onplayerclick();
	
		virtual void oncollision(obj* collidedwith);

		int id;

	};
	//determanes the type of object a give object is,
	enum objtype {
		block = 0,
		entity = 1

	};
	struct obj
	{


		array<component*> complist;

		//obj(v3::Vector3 ipos, const char* _name);

		
		objtype type;
		objstate state;
		template <class T>
		T& getcomponent();

		template <class T>
		T* getcomponentptr();
		template <class T>
		bool hascomponent();
		//removes a component
		template <class T>
		void removecomponent();

		


		template <class T, typename... types>
		void addcomponent(types&&... initval);
		template <class T, typename... types>
		T* addcomponentptr(types&&... initval);
		//todo -mid proiorty implement;
	
		obj();
		void senddestroycall() {

			for (int i = 0; i < complist.length; i++)
			{
				complist[i]->ondestroy();
			}
		}
	};


	void destroy(obj* object);
	inline bool shouldbeupdated(obj* object) {

		if (object->state == active)
		{
			return true;
		}
		if (object->state == beinginitiated)
		{
			object->state = active;
			return true;
		}
		return false;
	}

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
				complist[i]->ondestroy();
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


	template<class T>
	inline T* obj::getcomponentptr()
	{
		int id = compidfromname((char*)(typeid(T).name()));
		for (int i = 0; i < complist.length; i++)
		{

			if (id == complist[i]->id) {


				return ((T*)complist[i]);
			}
		}
		return nullptr;
	}

	template <class T>
	bool obj::hascomponent()
	{


		int id = compidfromname((char*)(typeid(T).name()));
		if (id == -1)
		{
	//no assert because it should be safe to do this 

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




	template<class T, typename ...types>
	inline T* obj::addcomponentptr(types && ...initval)
	{


		int id = idfromnameadd((char*)(typeid(T).name()));
		//fix
			//Assert(std::is_constructible_v<T, types...>, "no constructer takes these parameters");
		//	Assert(std::is_base_of<component, T>::value, "T is not a component");

		T* comp = new T(std::forward<types>(initval)...);

		comp->setobj(this);

		comp->start();
		comp->active = true;
		comp->id = compidfromname((char*)(typeid(T).name()));

		complist.append(comp);
		return comp;
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
		comp->active = true;
		complist.append(comp);

	}
	
	
	//is a guid with 2 numbers one for hashing and another for checking this basicly ellimiantes any prossiblity for collision as the other one can go to 2billion





	inline void immidiatedestroy(obj* object,bool soft) {
		
		if (soft)
		{
			object->state = beingsoftdestroyed;
		}
		else {


			object->state = beingroughdestroyed;
		}
		for (int i = 0; i < object->complist.length; i++)
		{
			object->complist[i]->ondestroy();
			//deletes component refered to by pointer
			if (!soft)
			{
				delete object->complist[i];
			}
		

		}
		if (!soft)
		{
			//deletes pointer itsekf
			object->complist.destroy();
			//makes it so "object from guid is now freed"
		}
	}


}
#endif#pragma once