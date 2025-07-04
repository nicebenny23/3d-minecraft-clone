#pragma once
#include "../util/dynamicarray.h"
#include "../world/tick.h"
#include "../util/chainpool.h"
#include <unordered_map>
#include "../util/SparseSet.h"
#include <typeindex>
#include "../util/vector3.h"
#include "../util/stack.h"
#include "transform.h"
#include <stdint.h>
#include "../util/type_index.h"
namespace GameContext {

}
template <typename T>
std::type_index get_type_index() {
	return std::type_index(typeid(T));
}
using namespace Cont;

namespace CtxName {
	struct Context;
}
namespace gameobject {

	



	enum updatecalltype {
		Framecall = 0,
		Rendercall = 1,


	};
	enum objstate : char
	{
		beinginitiated = 0,
		active = 1,
		destroying = 2,
	};
	enum updatetype : char {

		updatenone = 0,
		updatedefault = 1,
		updatetick = 2,
		updaterender = 3,
	};
	
	struct component;
	struct OCManager;
	struct EntityMetadata {
		array<component*> componentlist;
		
		objstate state;
		size_t gen_count;
		EntityMetadata() {
			gen_count=0;

		}
		void reset() {
			componentlist.destroy();
			gen_count++;
		}
		
	};
	struct obj
	{
		template <class T>
		T& getcomponent();

		template <class T>
		T* getcomponentptr();
		template <class T>
		bool hascomponent();

		
		//removes a component
		template <class T>
		void removecomponent();

	
		bool exists() const;
		uint32_t gen;
		 uint32_t Id;
		template <class T, typename... types>
		T* addcomponent(types&&... initval);
		//todo -mid proiorty implement;
		EntityMetadata& meta();
		array<component*>& componentlist();
	
		objstate& state();
		constexpr obj() noexcept {
			Id = 0;
			OC = nullptr;
			gen = 0;
		}
	
		void immediate_destroy();
		void deffered_destroy();
		Transform& transform();
	private:
		OCManager* OC;
		friend struct OCManager;
	};	
	static constexpr obj None  = obj();  // (1, 0, 0)
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
		void destroy(obj* object);
		OCManager() :ctx(nullptr){
			const size_t max_size = static_cast<size_t>(1) << 20;
			entitymeta = array<EntityMetadata>(max_size);
			free_ids = array<size_t>(max_size);
			for (size_t i = 0; i < max_size; i++)
			{
				free_ids.push(i);
				
			}
			managers = array<componentmanager, true>(0);
		
		}
		void inject_context(CtxName::Context* context)
		{
			ctx = context;
		}
		array<EntityMetadata> entitymeta;
		
		void Delete_deffered_objs();

		
		void delete_component(component* comp);
		 void InitObj(obj* object);
		 template <class T, typename... types>
		  T* InitComp(types&&... initval);
		 void updatecomponents(updatecalltype type);

		 obj CreateEntity(v3::Vector3 SpawnPos);
		 stackname::stack<obj> EntityDeletionBuffer;
		 type_id::dense_type_system comp_map;
	private:
	
		CtxName::Context* ctx;
		array<componentmanager, true> managers;
		array<size_t> free_ids;
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

			obj owner;

			unsigned int index;
			short priority=0;
		
		
			char comp_id;
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
				
				comp_id = 0;
				index = 0;
				ctx= nullptr;
			};
			void destroy(); 
			virtual void start();
			virtual void update();
			virtual void onplayerclick();

			virtual void oncollision(obj collidedwith);
			void SetManager(CtxName::Context* man)
			{
				ctx = man;
			}
		private:
			CtxName::Context* ctx;

		};
		template<typename T>
		void verify_component() {
			static_assert(std::derived_from<T, component>, "T must derive from ObjComponent<T>");
		}

		struct StaticComponent : gameobject::component
		{


		};
		struct transform_comp :gameobject::component {
			Transform transform;

		};

		
		
		

	
		template <class T>
		void obj::removecomponent()
		{
			//replace with a get components call

			size_t id = OC->comp_map.get<T>();
			
			
			for (int i = 0; i < componentlist().length; i++)
			{
				
				if (id == componentlist()[i]->comp_id) {
					componentlist()[i]->destroy();
					componentlist().deleteind(i);
					i--;

				}
			}
			return;
		}


		void destroy(obj* object);

		template<class T>
		inline T* obj::getcomponentptr()
		{

			size_t id = OC->comp_map.get<T>();
			
			array<component*>& complist = componentlist();
		
			for (int i = 0; i < complist.length; i++)
			{

				if (id == complist[i]->comp_id) {


					return ((T*)complist[i]);
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






		template <class T, typename... types>


		T* obj::addcomponent(types&&... initval)
		{


			//transfer this section over to the Update Manager
			T* comp = OC->InitComp<T>(std::forward<types>(initval)...);
			componentlist().push(comp);
			comp->owner = *this;
			comp->start();
			comp->active = true;
			return comp;
		}






	
	


		template<class T, typename ...types>
		inline T* OCManager::InitComp(types && ...initval)
		{

			verify_component<T>();
			T* comp;
			
			size_t id = comp_map.get<T>();
			bool newmanager = (managers[id].id == -1);
			if (newmanager)
			{
				managers[id].create(id, sizeof(T));

			}
			void* mem = (managers[id].pool.alloc());
			comp = new (mem) T(std::forward<types>(initval)...);
			comp->comp_id = id;
			comp->SetManager(ctx);


			if (newmanager)
			{

				managers[id].init(comp);

			}
			
			return comp;

		}
		inline bool operator == (const obj& obj1, const obj& obj2) {

			return (obj1.Id == obj2.Id) && (obj1.gen == obj2.gen)&&obj1.exists();
		}
}

