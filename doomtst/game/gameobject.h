#include "../util/vector3.h"
#include "../util/dynamicarray.h"
#include "../world/tick.h"
#include "../util/dynamicmempool.h"
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
	extern std::unordered_map<const char*, int> stringtoint;
	extern int curid;


	enum updatetype {

		updatenone = 0,
		updatedefault = 1,
		updatetick = 2,
	};

	struct component;

	struct componentmanager
	{
		componentmanager();
		void create(int mid,int bytesize);
		updatetype utype;
		int priority;
		int id;
		int componentamt;
		dynamicmempool::dynamicpool pool;
		array<component*> componentlist;
		void append(component* comp);
		void remove(int id);
		void init(component* sample);
	};



	struct obj;
	void initmanagerlist();
	extern array<componentmanager> managerlist;
	void updatecomponents();
	componentmanager* managerof(component* comp);
	inline void initmap() {



		stringtoint = std::unordered_map<const char*, int>();
		curid = 1;
	}

	inline	int idfromnameadd(const char* name) {
		int toret = stringtoint[name];
		if (toret==0)
		{
			stringtoint[name] = curid;
			curid++;
			return stringtoint[name];
		}
		return toret;
	}


	inline int compidfromname(const char* name) {
		int toret = stringtoint[name];
		if (toret == 0)
		{
			return -1;
		}
		return toret;
	}


inline 	bool shouldupdate(const updatetype& utype) {
		switch (utype)
		{
		case gameobject::updatenone:
			return false;
			break;
		case gameobject::updatedefault:
			return true;

			break;
		case gameobject::updatetick:
			if (tick::tickframe)
			{
				return true;
			}

		}
	}
		
		struct component
		{
		public:
		
			short priority;
			updatetype utype;
		

			unsigned int index;
			//for component manager 
			bool active;
			//called on destroy used for deallocation
			
			obj* owner;
			virtual void ondestroy();


			virtual ~component() = default;
			

			component() {
				active = true;
				owner = nullptr;
			//	id = -1;
				index = -1;
				utype = updatedefault;
			};
			void destroy() {
				ondestroy();
				managerlist[id].pool.free(this);
				if (managerof(this)->utype!=updatenone)
				{
					
					managerlist[id].remove(index);
				}
			}
			virtual void start();
			virtual void renderupdate();
			virtual void update();
			virtual void onplayerclick();

			virtual void oncollision(obj* collidedwith);

		unsigned short id;

		};
		//determanes the type of object a give object is,
		enum objtype {
			block = 0,
			entity = 1

		};

		struct obj
		{


			array<component*> componentlist;
			

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
		
			template <class T>
			array<T*>& getcomponents();

			
			template <class T, typename... types>
			T* addcomponent(types&&... initval);
			//todo -mid proiorty implement;

			obj();
			void senddestroycall() {

				for (int i = 0; i < componentlist.length; i++)
				{
					componentlist[i]->destroy();
				}
			}
		};


		void destroy(obj * object);
		template <class T>
		void obj::removecomponent()
		{
			int id = compidfromname((typeid(T).raw_name()));
			if (id == -1)
			{
				return;
			}
			for (int i = 0; i < componentlist.length; i++)
			{
				
				if (id == componentlist[i]->id) {
					componentlist[i]->destroy();
					componentlist.deleteind(i);
					i--;

				}
			}
			return;
		}



		template <class T>
		T& obj::getcomponent()
		{

			T* ptr = getcomponentptr<T>();
			if (ptr!=nullptr)
			{
				return *ptr;
			}
			Assert("owner does not have requested component");
		}
		template<class T>
		inline T* obj::getcomponentptr()
		{
			int id = compidfromname((typeid(T).raw_name()));
			for (int i = 0; i < componentlist.length; i++)
			{

				if (id == componentlist[i]->id) {


					return ((T*)componentlist[i]);
				}
			}
			return nullptr;
		}

		template <class T>
		bool obj::hascomponent()
		{


			int id = compidfromname((typeid(T).raw_name()));
			if (id == -1)
			{
				//no assert because it should be safe to do this 

				return false;
			}
			for (int i = 0; i < componentlist.length; i++)
			{

				if (id == componentlist[i]->id) {


					return true;
				}
			}
			return false;
		}




		template<class T>
		inline array<T*>& obj::getcomponents()
		{
			int id = compidfromname((typeid(T).raw_name()));
			if (id == -1)
			{
				Assert("compopnent does not exist");
			}
			array<T*>* comps = (new array<T*>());
			for (int i = 0; i < componentlist.length; i++)
			{
				if (id == componentlist[i]->id) {
					comps->append((T*)(componentlist[i]));
				}
			}
			return *comps;
		}


		template <class T, typename... types>


		T* obj::addcomponent(types&&... initval)
		{



			T* comp;
			 const char* idname = (typeid(T).raw_name());
			int id = idfromnameadd(idname);
			if (managerlist[id].id == -1)
			{
				managerlist[id].create(id, sizeof(T));
				void* mem = (managerlist[id].pool.allocate());
			comp = new (mem) T(std::forward<types>(initval)...);
				managerlist[id].init(comp);
			}
			else
			{
				void* mem = (managerlist[id].pool.allocate());
				comp = new (mem) T(std::forward<types>(initval)...);
			}
	
		

			comp->owner = this;
			comp->start();
			comp->active = true;
			comp->id = id;
		
			if (comp->utype!=updatenone)
			{
				managerof(comp)->append(comp);
			}
			componentlist.append(comp);
			return comp;

		}


		//is a guid with 2 numbers one for hashing and another for checking this basicly ellimiantes any prossiblity for collision as the other one can go to 2billion





		inline void immidiatedestroy(obj * object, bool soft) {

			if (soft)
			{
				object->state = beingsoftdestroyed;
			}
			else {


				object->state = beingroughdestroyed;
			}
			for (int i = 0; i < object->componentlist.length; i++)
			{
				object->componentlist[i]->destroy();
				

			}
			if (!soft)
			{
				//deletes pointer itsekf
				object->componentlist.destroy();
				//makes it so "object is now freed"
			}
		}
	


}

#endif