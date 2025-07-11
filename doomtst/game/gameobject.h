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
#include "../debugger/debug.h"
#include "../util/Id.h"
#include "../util/bitset.h"

#include "../util/pair.h"
#include "../util/type_index.h"
#include "Commands.h"
#include "Event.h"
#include "../util/dynpool.h"
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
namespace comp = type_id;
namespace gameobject {

	

	

	enum updatecalltype {
		Framecall = 0,
		Rendercall = 1,


	};
	struct component;
	struct Ecs;
	struct Archtype;
	
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


	struct ComponentMapper {
		constexpr size_t operator()(component* comp) const noexcept;
	};
	
	struct EntityMetadata {
	
		Archtype* arch;
		uint32_t arch_ind;
		uint32_t gen_count;
		
		EntityMetadata() {
			gen_count = 0;
			arch = nullptr;
			arch_ind = 0;
			
		}
		void reset() {
			arch = nullptr;
			arch_ind = 0;
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

		Ids::Id Id;

		template <class T, typename... types>
		T* addcomponent(types&&... initval);

		EntityMetadata& meta();
		

		constexpr obj() noexcept {
			Id = Ids::None;
			OC = nullptr;
		}

		void destroy();


		Transform& transform();

		Ecs* OC;
	private:



		friend struct Ecs;
	};
	static constexpr obj None = obj();

	struct Archtype {
		bitset::bitset bit_list;       
		array<size_t> dense_bits;     
		array<obj> elems;              
		array<Archtype*> moves;       
		Ecs* OC;
		template <typename T>
		bool has_component();
		bool has_component(comp::Id ind);
		bool has_components(bitset::bitset set);
		void add(obj& object) {
			EntityMetadata& met = object.meta();
			if (met.arch!=nullptr)
			{
				throw std::logic_error("Cannot add entity already in a Archytype to an archtype");
			}
			elems.push(object);
			met.arch = this;
			met.arch_ind = elems.length - 1;
		}
		void remove( obj& object) {
			EntityMetadata& met = object.meta();

			if (this != met.arch) {
				throw std::logic_error("Cannot remove a component from an archetype it is not a member of");
			}

			size_t index = met.arch_ind;
			size_t end_index = elems.length - 1;

			if (index != end_index) {
				obj& moved = elems[end_index];
				moved.meta().arch_ind = index;
				std::swap(elems[index], elems[end_index]);
			}
			
			met.arch = nullptr;
			elems.pop();
			if (elems.length<100)
			{

				for (auto& element : elems)
				{

					if (element.Id == object.Id) {
						throw std::logic_error("skill");
					}
				}
			}


		}
	
		Archtype(bitset::bitset st,Ecs* Man) :bit_list(st), OC(Man),elems() {

			for (size_t ind = 0; ind < st.bits; ind++) {
				if (bit_list[ind]) {
					dense_bits.push(ind);
				}   
				for (size_t i = 0; i < st.bits; ++i) {
					moves[i] = nullptr;
				}
			}
		};
		Archtype() {
			OC = nullptr;
			for (size_t i = 0; i < moves.length; ++i) {
				moves[i] = nullptr;
			}
			bit_list = bitset::None;
		}
		

		using iterator = typename Cont::array<obj>::iterator;

		iterator begin() { return elems.begin(); }
		iterator end() { return elems.end(); }
		
	};


	using componentStorage = array<component*>;
	struct componentmanager
	{
		componentmanager();
		void create(comp::Id mid, size_t bytesize,size_t alignment);
		comp::Id id;             
		dynPool::flux<component> pool;
		componentStorage store; 
		updatetype utype;     
		int priority;         
		
		void init(component* sample);
	};
	
	
	struct ArchtypeManager {
		ArchtypeManager() {
			OC = nullptr;
		}
		ArchtypeManager(Ecs* man) :OC(man) {
			archtypes.push(new Archtype(bitset::None, OC));
		};
		bool check()  {
			for (Archtype* arch : archtypes) {
				if (!arch) continue;
				if (arch->elems.length == 1 && arch->elems[0].meta().arch != arch) {
					return true;
				}
			}
			return false;
		}
		Archtype& operator[](size_t index) {
		return *archtypes[index];
		}
		size_t length() {
			return archtypes.length;
		}
		array<Archtype*> archtypes; 
		void expandArchtype() {

			for (auto& arch : archtypes)
			{
				arch->bit_list.push(false);
	
			}

		}

		void addArchtype(bitset::bitset Components) {

			check();
			for (Archtype* arch : archtypes) {
				if (arch->bit_list == Components) {
					throw std::logic_error("cannot have duplicate archtypes in ecs");
				}
			}
			archtypes.push(new Archtype(Components, OC));
			for (auto& arch : archtypes)
			{
				bitset::bitset archxor = Components ^ arch->bit_list;
				if (archxor.popcount() == 1)
				{
					for (size_t j = 0; j < archxor.bits; j++)
					{
						if (archxor[j])
						{

							archtypes[archtypes.length - 1]->moves[j] = arch;
							arch->moves[j] = archtypes[archtypes.length - 1];
						}
					}
				}
			}

			check();
		}

		void moveflipArch(obj& object, comp::Id index) {

			check();
			Archtype* current = object.meta().arch;
			if (current == nullptr)
			{
				throw std::logic_error("Component must be part of an archtype");
			}

			Archtype* new_type = current->moves[index.value];
			if (new_type == nullptr)
			{
				bitset::bitset new_arch = current->bit_list;

				new_arch.flip(index.value);
				addArchtype(new_arch);
				new_type = current->moves[index.value];
				if (new_type==nullptr)
				{
					throw std::logic_error("Improper archytype allocation");
				}
			}
			current->remove(object);
			new_type->add(object);

			check();
			if (current->elems.length< 100)
			{

				for (auto& element : current->elems)
				{

					if (element.Id == object.Id) {
						throw std::logic_error("skill");
					}
				}
			}
		}

		using iterator = typename Cont::array<Archtype*>::iterator;
		iterator begin() { return archtypes.begin(); }
		iterator end() { return archtypes.end(); }
	private:
		Ecs* OC;
	};


	struct Ecs {
		CommandBuffer commands;
		event::EventBus events;
		template<typename EventType>
		void Call_Event(const EventType& Event){
			events.Call<EventType>(Event);
		}
		template<typename EventType,typename Listener>
		void Add_listener() {
			events.Add_listener<EventType, Listener>();
		}


		Ecs(){
			commands = CommandBuffer(this);
			arch = ArchtypeManager(this);
			const size_t max_size = static_cast<size_t>(1) << 19;
			ctx = nullptr;
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
	
		template<typename T>
		T* getcomponentptr(obj& object);
		
		obj CreateEntity(v3::Vector3 SpawnPos);
		void InitObj(obj& object);

		void destroy(obj& object);
		
		
		  template <class T, typename... types>
		  T* InitComp(types&&... initval);

		  void updatecomponents(updatecalltype type);
		 		 void delete_component(component* comp);

		 Cont::stack<obj> EntityDeletionBuffer;
		 
		
		 type_id::dense_type_system comp_map;
		 ArchtypeManager arch;
		 array<componentmanager, true> managers;
	private:
	
		CtxName::Context* ctx;
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
			comp::Id comp_id;        
			obj owner;
			short priority;         
			updatetype utype;
			//called on destroy used for deallocation
			
			virtual void ondestroy();
			//use for blockmove
			//virtual void transfer(component& obj);

			virtual ~component() = default;
			component() {
				utype = updatedefault;
				comp_id = comp::None;
			};
			void destroy(); 
			virtual void start();
			virtual void update();
			virtual void onplayerclick();
			virtual void oncollision(obj collidedwith);
	

		};
		template<typename T>
		constexpr void verify_component() {
			static_assert(std::derived_from<T, component>, "T must derive from ObjComponent<T>");
		}

		struct StaticComponent : gameobject::component
		{


		};
		struct transform_comp : gameobject::component {
			Transform transform;

		};

		
	

	
		template <class T>
		void obj::removecomponent()
		{
			//replace with a get components call
			verify_component<T>();

			OC->arch.check();
			comp::Id comp_id = OC->comp_map.get<T>();

			auto& complist = OC->managers[comp_id.value].store;
			if (complist[Id.id]!=nullptr) {
				OC->arch.moveflipArch(*this, comp_id);
				component* comp = complist[Id.id];
				if (!comp) {
					throw std::logic_error("invarient violation:component must exist if id is contained");
				}
				comp->destroy();

			}
			
		}


		void destroy(obj* object);
	
		template<class T>
		T* Ecs::getcomponentptr(obj& object)
		{
			verify_component<T>();

			arch.check();
			comp::Id comp_id = comp_map.get<T>();
			componentStorage& complist = managers[comp_id.value].store;

			arch.check();
			return (T*)(complist[object.Id.id]);

		}
		template<class T>
		T* obj::getcomponentptr()
		{
			OC->arch.check();
			return OC->getcomponentptr<T>(*this);
		}
		template <class T>
		T& obj::getcomponent()
		{
			verify_component<T>();

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
			verify_component<T>();

			OC->arch.check();
			return meta().arch->has_component(OC->comp_map.get<T>());
			
		}






		template <class T, typename... types>
		T* obj::addcomponent(types&&... initval)
		{

			verify_component<T>();
			T* comp = OC->InitComp<T>(std::forward<types>(initval)...);
			comp->owner = *this;
			auto& complist = OC->managers[comp->comp_id.value].store;
			
			complist[Id.id]=comp;
			OC->arch.moveflipArch(*this, comp->comp_id);

			
			comp->start();

			OC->arch.check();
			//Assert(complist, "Archtype does not contain needed key");
			return comp;
		}



		template<class T, typename ...types>
		inline T* Ecs::InitComp(types && ...initval)
		{

			verify_component<T>();
			T* comp;
			
			auto [cmpid,is_new]  = comp_map.insert<T>();
			componentmanager& man = managers[cmpid.value];
		
			if (is_new)
			{
				arch.expandArchtype();
				man.create(cmpid, sizeof(T),alignof(T));
			
			}
			void* mem = man.pool.alloc();
			comp = new (mem) T(std::forward<types>(initval)...);
			comp->comp_id = cmpid;
		

			if (is_new)
			{
				man.init(comp);

			}
			
			return comp;

		}
		inline bool operator == (const obj& obj1, const obj& obj2) {

			return obj1.Id == obj2.Id;
		}
		inline bool gameobject::Archtype::has_component(comp::Id index)
		{
			if (index.value<bit_list.bits)
			{
				return bit_list.at(index.value);
			}
			return false;
		}
		inline bool Archtype::has_components(bitset::bitset set)
		{
			return (bit_list & set) == set;
		}
		template<typename T>
		bool Archtype::has_component()
		{
		return has_component(OC->comp_map.get<T>());
		}

		//will not care if component list has been changed during the iteration




		

		

}

