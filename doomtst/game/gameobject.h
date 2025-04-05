#include "../util/vector3.h"
#include "../util/dynamicarray.h"
#include "../world/tick.h"
#include "../util/chainpool.h"
#include <unordered_map>
#pragma once


using namespace dynamicarray;
#ifndef gameobj_HPP

#define gameobj_HPP

namespace gameobject {

	enum objstate : byte
	{
		beinginitiated=0,
		active=1,
		destroying=2,
	};

	//these 3 function are used to get a unique component number f
	extern std::unordered_map<const char*, int> stringtoint;
	extern int curid;


	enum updatetype : byte {

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
		chainpool::chainedpool<component> pool;
		

		void init(component* sample);
	};



	struct obj;
	void initmanagerlist();
	extern array<componentmanager,true> managerlist;
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

			obj* owner;

			unsigned int index;
			short priority=0;
		

			byte id;
			//for component manager 
			updatetype utype = updatedefault;

			bool active;
			//called on destroy used for deallocation
			
			virtual void ondestroy();
			//use for blockmove
			//virtual void transfer(component& obj);

			virtual ~component() = default;
			

			component() {
				active = true;
				owner = nullptr;
				id = -1;
				index = -1;
				
			};
			void destroy() {
				ondestroy();
				managerlist[id].pool.free(this);
						}
			virtual void start();
			virtual void renderupdate();
			virtual void update();
			virtual void onplayerclick();

			virtual void oncollision(obj* collidedwith);


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


		void destroy(obj* object);

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
		T& obj::getcomponent()
		{

			T* ptr = getcomponentptr<T>();
			if (ptr == nullptr)
			{
				throw std::invalid_argument("object does not requested component");
			}

			return *ptr;

		}
		template <class T>
		bool obj::hascomponent()
		{


			T* ptr = getcomponentptr<T>();
			return (ptr != nullptr);
			
		}




		template<class T>
		inline array<T*>& obj::getcomponents()
		{
			int id = compidfromname(typeid(T).raw_name());
			
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
			bool newmanager = (managerlist[id].id == -1);
			if (newmanager)
			{
				managerlist[id].create(id, sizeof(T));
			
			}
			void* mem = (managerlist[id].pool.alloc());
			comp = new (mem) T(std::forward<types>(initval)...);
			if (newmanager)
			{

				managerlist[id].init(comp);

			}
		

			comp->owner = this;
			comp->start();
			comp->active = true;
			comp->id = id;
		
		
			componentlist.append(comp);
			return comp;

		}






		inline void destroycomponents(obj * object) {

			

				object->state = destroying;
			
			for (int i = 0; i < object->componentlist.length; i++)
			{
				object->componentlist[i]->destroy();
				

			}
			
				
				object->componentlist.destroy();
				
		}
	


}

#endif