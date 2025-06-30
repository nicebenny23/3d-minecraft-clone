#pragma once
#include "../util/dynamicarray.h"
#include "../world/tick.h"
#include "../util/chainpool.h"
#include <unordered_map>


namespace GameContext {

}
using namespace Cont;

namespace CtxName {
	struct Context;
}
namespace gameobject {
	enum updatecalltype {
		Framecall = 0,
		Rendercall=1,


	};
	enum objstate : byte
	{
		beinginitiated=0,
		active=1,
		destroying=2,
	};
	enum updatetype : byte {

		updatenone = 0,
		updatedefault = 1,
		updatetick = 2,
		updaterender = 3,
	};

	struct component;

	struct obj;
	struct componentmanager
	{
		componentmanager();
		void create(int mid, int bytesize);
		updatetype utype;
		int priority;
		int id;
		chainpool::chainedpool<component> pool;
		  

		void init(component* sample);
	};
	struct OCManager {

		OCManager() {


			managers = array<componentmanager, true>(0);
			CompHasher = std::unordered_map<const char*, int>();
			compid = 1;
			ObjId = 1;
		}

		CtxName::Context* ctx;
		array<componentmanager, true> managers;
		int compid;
		size_t ObjId;
		std::unordered_map<const char*, int> CompHasher;
		int GetCompIdAdd(const char* name) {
			int compId = CompHasher[name];
			if (compId == 0)
			{
				CompHasher[name] = compid;
				compid++;
				return CompHasher[name];
			}
			return compId;
		}


		 int GetCompId(const char* name) {
			int compId = CompHasher[name];
			if (compId== 0)
			{
				return -1;
			}
			return compId;
		 }
		 void InitObj(obj* object);
		 template <class T, typename... types>
		  T* InitComp(types&&... initval);
		 void updatecomponents(updatecalltype type);
	};


	




inline 	bool shouldupdate(const updatetype& utype,updatecalltype calltype) {
	if ( calltype == Rendercall)
	{
		return (utype == gameobject::updaterender);
	}
	else
	{


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
			break;
		case updaterender:
			return false;
			break;
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
				ctx= nullptr;
			};
			void destroy(); 
			virtual void start();
			virtual void update();
			virtual void onplayerclick();

			virtual void oncollision(obj* collidedwith);
			void SetManager(CtxName::Context* man)
			{
				ctx = man;
			}
		private:
			CtxName::Context* ctx;

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
			

			size_t id;
			template <class T, typename... types>
			T* addcomponent(types&&... initval);
			//todo -mid proiorty implement;

			obj();
			void DestroyComponents() {

				for (int i = 0; i < componentlist.length; i++)
				{
					componentlist[i]->destroy();
				}
			}
		
			
		private:
			
				OCManager* OC;
				friend struct OCManager;
		};


	
		template <class T>
		void obj::removecomponent()
		{
			
			//replace with a get components call

			int id = OC->GetCompId((typeid(T).raw_name()));
			
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
			int id = OC->GetCompId((typeid(T).raw_name()));
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
				throw std::invalid_argument("object does not have requested component");
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
			
			int id = OC->GetCompId(typeid(T).raw_name());
			
			array<T*>* comps = (new array<T*>());
			for (int i = 0; i < componentlist.length; i++)
			{
				if (id == componentlist[i]->id) {
					comps->push((T*)(componentlist[i]));
				}
			}
			return *comps;
		}


		template <class T, typename... types>


		T* obj::addcomponent(types&&... initval)
		{


			//transfer this section over to the Update Manager
			T* comp = OC->InitComp<T>(std::forward<types>(initval)...);




			componentlist.push(comp);
			comp->owner = this;
			
			comp->start();
			comp->active = true;
			
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
	


		template<class T, typename ...types>
		inline T* OCManager::InitComp(types && ...initval)
		{
		//find a way to fix it so i dont have to init the manager extremly late.
		//onr 
			T* comp;
			const char* idname = (typeid(T).raw_name());
			int id = GetCompIdAdd(idname);
			bool newmanager = (managers[id].id == -1);
			if (newmanager)
			{
				managers[id].create(id, sizeof(T));

			}
			void* mem = (managers[id].pool.alloc());
			comp = new (mem) T(std::forward<types>(initval)...);
			comp->id = id;
			comp->SetManager(ctx);


			if (newmanager)
			{

				managers[id].init(comp);

			}
			
			return comp;

		}

}

