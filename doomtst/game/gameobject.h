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
#include <stdexcept>
#include "../util/pair.h"
#include "../util/index.h"
#include "../util/type_index.h"
#include "Commands.h"
#include "Event.h"
#include "../util/counter.h"
#include "../util/dynpool.h"

namespace GameContext {
}

using namespace stn;
namespace CtxName {
	struct Context;
}
namespace comp = Ids;
namespace gameobject {
	
	


	enum updatecalltype {
		Framecall = 0,
		Rendercall = 1,


	};
	enum updatetype : char {

		updatenone = 0,
		updatedefault = 1,
		updatetick = 2,
		updaterender = 3,
	};


	
	struct ArchtypeIndexTag {};
	using ArchtypeIndex =stn::typestate::typed_index<ArchtypeIndexTag>;

	struct Archtype_tag {};
	using arch_id = Ids::typed_id<Archtype_tag>;
	struct EntityMetadata {

		arch_id arch;
		ArchtypeIndex arch_ind;
		uint32_t gen_count;

		EntityMetadata():arch(0),arch_ind(0){
			gen_count = 0;
			

		}
		void reset() {
			arch = arch_id(0);
			arch_ind = ArchtypeIndex(0);
			gen_count++;

		}

	};

	struct Ecs;
	struct obj
	{
		template <class T>
		T& getcomponent();
		template <class T>
		void ensure_component();
		template <class T>
		T* getcomponentptr();

		template <class T>
		bool hascomponent();

		//removes a component
		template <class T>
		void removecomponent();

		inline bool exists() const;

		
		constexpr std::uint32_t id() const {
			return Genid.id;
		}
		constexpr size_t generation() const{
	
			return Genid.gen;
		}
	
		template <class T, typename... types>
		T* addcomponent(types&&... initval);

		inline EntityMetadata& meta();
	
		constexpr obj() noexcept :Genid() {
			
			OC = nullptr;
		}
	//	constexpr obj(const entity& Entity, Ecs* World) : Genid(Entity.id().id, Entity.generation().gen()), OC(World) {};
		void destroy();
		//entity inner() const{
		//	return entity(entity_id(id()), Generation(generation()));
		//}

		Transform& transform();
		
		Ecs* OC;
	private:
		friend bool operator==(const obj& lhs, const obj& rhs) {
			return lhs.Genid == rhs.Genid;
		}
		friend bool operator!=(const obj& lhs, const obj& rhs) {
			return lhs.Genid != rhs.Genid;
		}
		Ids::GenId Genid;
		friend struct Ecs;
	};
	static constexpr obj None = obj();
	

	struct component
	{

		obj owner;
		comp::Id comp_id;
		short priority;
		updatetype utype;
		//called on destroy used for deallocation

		virtual void destroy_hook();
		virtual ~component() = default;
		component() {
			owner = None;
			priority = 0;
			utype = updatedefault;
			comp_id = comp::None;
		};
		void destroy();
		virtual void start();
		virtual void update();
		virtual void onplayerclick();
		virtual void oncollision(obj collidedwith);
		template<typename T>
		T& get_component() {
			return owner.getcomponent<T>();
		}
		template<typename T>
		bool has_component(){
			return owner.hascomponent<T>();
		}

	};
	struct Archtype {
		bitset::bitset bit_list;
		
		array<obj> elems;
		array<Option<arch_id>> moves;
		template <typename T>
		bool has_component();
		bool has_component(comp::Id ind);
		bool has_components(bitset::bitset set) const;
		bool has_any(bitset::bitset set) const;
		
		void add(obj& object) {
			EntityMetadata& met = object.meta();
			
			elems.push(object);
			met.arch=Id;
			met.arch_ind = ArchtypeIndex(elems.length() - 1);
		}
		void remove(obj& object) {
			EntityMetadata& met = object.meta();
			if (elems.empty()) {
				throw std::logic_error("Cannot remove an entity from an empty archetype");
			}
			if (Id!= met.arch) {
				throw std::logic_error("Cannot remove an entity from an archetype it does not reside in");
			}
			
			size_t index = met.arch_ind.index;
			elems.last().meta().arch_ind= ArchtypeIndex(index);
			elems.swap_drop(index);
			met.arch = arch_id(-1);
		}

		Archtype(bitset::bitset st, Ecs* Man,arch_id id) :bit_list(st), OC(Man), elems(),Id(id) {

			for (size_t ind = 0; ind < st.bits; ind++) {
				moves.push(stn::None);
			}
		};
		obj& operator[](std::uint32_t arch_index) {
			return elems[arch_index];
		}
		const obj& operator[](std::uint32_t arch_index) const {
			return elems[arch_index];
		}

		std::uint32_t count() const{
			return elems.length();
		}
		std::uint32_t id() const {
			return Id.id;	
		}
		
		Option<arch_id> flip_id(comp::Id index) const{
			if (moves.contains_index(index.id))
			{
				return moves[index.id];
			}
			return stn::None;
		}
		using iterator = typename stn::array<obj>::iterator;

		iterator begin() { return elems.begin(); }
		iterator end() { return elems.end(); }
	private:
		arch_id Id;
		Ecs* OC;
	};


	
	// A list of Archtypes
	struct Archtypes {
		Archtypes() {
			OC = nullptr;
		}
		Archtypes(Ecs* man) :OC(man) {
			addArchtype(bitset::bitset());
		};
		inline Archtype& operator[](size_t index) {
			return *archtypes[index];
		}
		inline const Archtype& operator[](size_t index) const{
			return *archtypes[index];
		}
		Archtype& operator[](arch_id Id) {
			return *archtypes[Id.id];
		}
		const Archtype& operator[](arch_id Id) const{
			return *archtypes[Id.id];
		}
		
		inline size_t length() const{
			return archtypes.length();
		}
		const Archtype& empty_archtype() const{
			return *archtypes[0];
		}
		Archtype& empty_archtype() {
			return *archtypes[0];
		}
		void expandArchtype() {

			for (auto& arch : archtypes)
			{
				arch->bit_list.push(false);
				arch->moves.expand(arch->moves.length() + 1);
			}

		}
		
		void addArchtype(const bitset::bitset& Components) {

		
			for (Archtype* arch : archtypes) {
				if (arch->bit_list == Components) {
					throw std::logic_error("ecs cannot contain duplicate archtypes");
				}
			}
			archtypes.push(new Archtype(Components, OC,arch_id(archtypes.length())));

			for (auto& arch : archtypes)
			{
				bitset::bitset archxor = Components ^ arch->bit_list;
				if (archxor.popcount() == 1)
				{
					for (size_t j = 0; j < archxor.bits; j++)
					{
						if (archxor[j])
						{

							archtypes.last()->moves[j] = arch->id();
							arch->moves[j] = archtypes.last()->id();
						}
					}
				}
			}

			
		}

		void transfer_object(obj& object, comp::Id index) {

		
			Archtype* current = archtypes[object.meta().arch.id];
			if (current == nullptr)
			{
				throw std::logic_error("Component must be part of an archtype");
			}
			Option<arch_id> id =  current->flip_id(index);
			if (!id)
			{
				bitset::bitset new_arch = current->bit_list;

				new_arch.flip(index.id);
				addArchtype(new_arch);
				//since it now exists
				id=current->flip_id(index);
				if (!id)
				{
					throw std::logic_error("Improper archytype allocation");
				}
			}
			current->remove(object);
			archtypes[(*id).id]->add(object);


		}

		using iterator = typename stn::array<Archtype*>::iterator;
		iterator begin() { return archtypes.begin(); }
		iterator end() { return archtypes.end(); }

	private:
		util::counter archtype_counter;
		array<Archtype*> archtypes;
		Ecs* OC;
	};


	struct component_description {
		short priority;
		updatetype update_type;
		component_description(short component_priority, updatetype utype) :priority(component_priority), update_type(utype) {}
		component_description() {
			priority = -1;
			update_type = updatenone;
		}
	};
	using componentStorage = array<component*>;
	struct component_table
	{
		component_table(comp::Id mid, stn::memory::layout layout);
		stn::memory::layout component_layout;
		dynPool::flux<component> pool;
		componentStorage store;
		comp::Id id;
		component_description description;
		component*& operator[](const obj& entity) {
			return store.reach(entity.id());
		}
		const component* operator[](const obj& entity) const {
			return store[entity.id()];
		}
		bool contains(const obj& entity) const {
			return store[entity.id()] != nullptr;
		}
		void init(component* sample);
	};
	struct Ecs {
		CommandBuffer commands;
		event::EventBus events;
		template<typename EventType>
		void Call_Event(const EventType& Event) {
			events.Call<EventType>(Event);
		}
		template<typename EventType, typename Listener>
		void Add_listener() {
			events.Add_listener<EventType, Listener>();
		}
		template <typename... Components>
		bitset::bitset bitset_of() {
			array<size_t> indices = component_indexer.get_type_ids<Components>();
			bitset::bitset res;
			for (auto ind:indices)
			{
				res.expand(ind);
				res.set(ind);
			}
			return res;
		}

		Ecs() {
			commands = CommandBuffer(this);
			arch = Archtypes(this);
			const size_t max_size = static_cast<size_t>(1) << 19;
			ctx = nullptr;
			entitymeta = array<EntityMetadata>(max_size);

			free_ids = array<uint32_t>(max_size);
			for (size_t i = 0; i < max_size; i++)
			{
				free_ids.push(i);

			}
			comp_storage = array<component_table>();

		}
		void inject_context(CtxName::Context* context)
		{
			ctx = context;
		}
		array<EntityMetadata> entitymeta;
		obj create_entity()
		{
			return  spawn_empty();
			

		}

		template<typename T>
		T* getcomponentptr(obj& object);
		template<typename T>
		T& getcomponent(obj& object);
		obj spawn_with_transform(v3::Vec3 SpawnPos);
		obj spawn_empty();

		void destroy(obj& object);

		template<typename... Components>
		std::tuple<Components*...> get_tuple(obj& obj, stn::array<comp::Id>& indices);
		template <class T, typename... types>
		T* add_component(obj& entity,types&&... initval);

		void updatecomponents(updatecalltype type);
		void delete_component(component* comp);
		template <class T, typename... types>
		component* create_component(obj& entity, types&&... initval);
		void bind_component(component* comp,obj& entity);
		type_id::type_indexer component_indexer;
		Archtypes arch;
		Archtype& operator[](arch_id index) {
			return arch[index];
		}
		array<component_table> comp_storage;
	private:
		component_table& component_table_with_id(comp::Id id) {
			return comp_storage[id.id];
		}
		CtxName::Context* ctx;
		array<uint32_t> free_ids;
	};




	inline 	bool shouldupdate(const updatetype& utype, updatecalltype calltype) {
		if (calltype == Rendercall)
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
	struct CompponentMetaData {
		bool is_tag;
		float priority;
		updatetype utype;
	};
	
	
	template<typename T>
	inline constexpr void verify_component() {
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
		if (!hascomponent<T>()) {
			return;
		}
		Option<Ids::Id> comp_id = OC->component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return;
		}

		OC->arch.transfer_object(*this, *comp_id);
		if (!OC->comp_storage[(*comp_id).id].contains(*this)) {
			throw std::logic_error("invariant violation:component must exist if id is contained");
		}
		OC->comp_storage[(*comp_id).id][*this]->destroy();

			
		

	}


	template<class T>
	T* Ecs::getcomponentptr(obj& object)
	{
		verify_component<T>();
	
	
		Option<comp::Id> comp_id = component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return nullptr;
		}
		componentStorage& complist = component_table_with_id(comp_id.unwrap()).store;

		return (T*)(complist[object.id()]);

	}
	template<class T>
	inline T* obj::getcomponentptr()
	{
		if (!OC)
		{
			throw std::logic_error("Object Not initilized");
		}
		return OC->getcomponentptr<T>(*this);
	}

	template<class T>
	T& Ecs::getcomponent(obj& object)
	{
		verify_component<T>();


		comp::Id comp_id = component_indexer.get<T>();

		component* comp= component_table_with_id(comp_id)[object];
		if (comp==nullptr)
		{
			throw std::logic_error("Entity does not have component" + std::string(typeid(T).name()));
		}
		return *(T*)comp;

	}
	template <class T>
	T& obj::getcomponent() 
	{
		if (!OC)
		{
			throw std::logic_error("Object Not initilized");
		}
		return OC->getcomponent<T>(*this);
	}
	
	template <class T>
	inline bool obj::hascomponent() 
	{
		verify_component<T>();
		Option<comp::Id> id = OC->component_indexer.get_opt<T>();
		if (!id)
		{
			return false;
		}
		return  (*OC)[meta().arch].has_component(*id);

	}
	template<class T>
	inline void obj::ensure_component()
	{
		if (!hascomponent<T>())
		{
			throw std::logic_error("Entity does not have component" + std::string(typeid(T).name()));
		}
	}
	inline void gameobject::Ecs::bind_component(component* comp, obj& entity)
	{
		component_table& row = component_table_with_id(comp->comp_id);
		comp->owner = entity;
		
		row[entity] = (component*)(comp);
		arch.transfer_object(entity, comp->comp_id);
		comp->start();
	}


	template<class T, typename ...Args>
	inline component* Ecs::create_component(obj& entity, Args&& ...args) 
	{
		auto [cmpid, is_new] = component_indexer.insert<T>();


		
		if (is_new)
		{
			component_table& row = comp_storage.emplace(cmpid, stn::memory::layout_of<T>);

			row.store.expand(entitymeta.cap());
			T* comp = new (row.pool.alloc()) T(std::forward<Args>(args)...);
			comp->comp_id = cmpid;
			arch.expandArchtype();
			row.init(comp);

		}
		else
		{
			component_table& row = component_table_with_id(cmpid);
			if (row[entity] != nullptr)
			{
				return (T*)row[entity];
			}
			T* comp = new (row.pool.alloc()) T(std::forward<Args>(args)...);
			comp->comp_id = cmpid;
		}

		return comp;
	}
	template<class T, typename ...types>
	inline T* Ecs::add_component(obj& entity,types && ...initval)
	{
		static_assert(std::is_constructible_v<T, types...>, "no constructor for T takes in [types...]");
		component* comp =create_component<T,types...>(entity, std::forward<types>(initval)...);
		bind_component(comp, entity);
		return (T*)comp;

	}
	
	template <class T, typename... types>
	T* obj::addcomponent(types&&... initval)
	{
		return OC->add_component<T>(*this,std::forward<types>(initval)...);
	}

	
	inline bool gameobject::Archtype::has_component(comp::Id index)
	{
		
		if (bit_list.contains_index(index.id))
		{
			
			return bit_list.at(index.id);
		}
		return false;
	}
	inline bool Archtype::has_components(bitset::bitset set) const
	{
		return (bit_list & set) == set;
	}
	inline bool Archtype::has_any(bitset::bitset set) const
	{
		return bit_list.matches_any(set);
	}
	template<typename T>
	bool Archtype::has_component()
	{
		Option<comp::Id> comp_id = OC->component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return false;
		}
		return has_component(*comp_id);
		//will not care if component list has been changed during the iteration
	}
	
	//just trust this works
	template<typename... Components>
	std::tuple<Components*...> Ecs::get_tuple(obj& obj,stn::array<comp::Id>& indices) {
		

		return[&]<size_t... Is>(std::index_sequence<Is...>) {
			return std::tuple<Components*...>{
				(Components*)comp_storage[indices[Is].id][obj]...
			};
		}(std::index_sequence_for<Components...>{});
	}
}