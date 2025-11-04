#pragma once
#include "../world/tick.h"
#include "../util/chainpool.h"
#include "../util/SparseSet.h"
#include <typeindex>
#include "../util/vector3.h"
#include "transform.h"
#include "../debugger/debug.h"
#include "../util/Id.h"
#include "../util/bitset.h"
#include <stdexcept>
#include "../util/index.h"
#include "../util/type_index.h"
#include "Commands.h"
#include "Event.h"
#include "../util/counter.h"
#include "../util/Dynpool.h"
#include "ecs/entity.h"
#include "ecs/component_id.h"
namespace GameContext {
}

using namespace stn;
namespace CtxName {
	struct Context;
}
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


	//index of an element within an archetype
	struct ArchtypeIndexTag {};
	using archetype_index =stn::typestate::typed_index<ArchtypeIndexTag>;
	//index of a distint archetypr
	struct Archtype_tag {};
	using archetype_id = Ids::typed_id<Archtype_tag>;
	struct EntityMetadata {

		archetype_id arch;
		archetype_index arch_ind;
		uint32_t gen_count;

		EntityMetadata():arch(0),arch_ind(0){
			gen_count = 0;
			

		}
		void clear() {
			arch = archetype_id(0);
			arch_ind = archetype_index(0);
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
			return entity.id();
		}
		constexpr size_t generation() const{
	
			return entity.generation();
		}
	
		template <class T, typename... types>
		T* addcomponent(types&&... initval);

		inline EntityMetadata& meta();
	
		constexpr obj() noexcept :entity() {
			
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
			return lhs.entity == rhs.entity;
		}
		friend bool operator!=(const obj& lhs, const obj& rhs) {
			return lhs.entity != rhs.entity;
		}
		ecs::entity entity;
		friend struct Ecs;
	};
	constexpr obj None = obj();
	

	struct component
	{

		obj owner;
		ecs::component_id comp_id;
		short priority;
		updatetype utype;
		//called on destroy used for deallocation

		virtual void destroy_hook();
		//fix
		virtual ~component() = default;
		component():comp_id(1<<23){
			owner = None;
			priority = 0;
			utype = updatedefault;

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
		stn::bitset bit_list;
		
		array<obj> elems;
		array<Option<archetype_id>> moves;
		template <typename T>
		bool has_component();
		bool has_component(ecs::component_id ind);
		bool has_components(stn::bitset set) const;
		bool has_any(stn::bitset set) const;
		
		void add(obj& object) {
			elems.push(object);

			EntityMetadata& met = object.meta();
			met.arch=Id;
			met.arch_ind = archetype_index(elems.length() - 1);
		}
		void remove(obj& object) {
		
			if (elems.empty()) {
				throw std::logic_error("Cannot remove an entity from an empty archetype");
			}
			EntityMetadata& met = object.meta();
			if (Id!= met.arch) {
				throw std::logic_error("Cannot remove an entity from an archetype it does not reside in");
			}
			
			size_t index = met.arch_ind.index;
			elems.last().meta().arch_ind= archetype_index(index);
			elems.swap_drop(index);
		
		}
		void remove(archetype_index index) {

			if (!elems.contains_index(index.index)) {
				//complete
				throw std::logic_error("Cannot remove entity");
			}
			elems.swap_drop(index.index);
		}   

		Archtype(stn::bitset st, Ecs* Man,archetype_id id) :bit_list(st), OC(Man), elems(),Id(id) {

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
		
		Option<archetype_id> flip_id(ecs::component_id index) const{
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
		archetype_id Id;
		Ecs* OC;
	};


	
	// A list of Archtypes
	struct Archtypes {
		Archtypes() {
			OC = nullptr;
		}
		Archtypes(Ecs* man) :OC(man) {
			addArchtype(stn::bitset());
		};
		inline Archtype& operator[](size_t index) {
			return *archtypes[index];
		}
		inline const Archtype& operator[](size_t index) const{
			return *archtypes[index];
		}
		Archtype& operator[](archetype_id Id) {
			return *archtypes[Id.id];
		}
		const Archtype& operator[](archetype_id Id) const{
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
				arch->bit_list.expand(false);
				arch->moves.expand(arch->moves.length() + 1);
			}

		}
		
		void addArchtype(const stn::bitset& Components) {

		
			for (Archtype* arch : archtypes) {
				if (arch->bit_list == Components) {
					throw std::logic_error("ecs cannot contain duplicate archtypes");
				}
			}
			archtypes.push(new Archtype(Components, OC,archetype_id(archtypes.length())));

			for (auto& arch : archtypes)
			{
				stn::bitset archxor = Components ^ arch->bit_list;
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
		void drop(obj& object) {

			EntityMetadata e_meta= object.meta();
			archetype_index index = e_meta.arch_ind;
			Archtype& stored = *archtypes[e_meta.arch.id];
			stored[stored.count() - 1].meta().arch_ind= index;
			stored.remove(index);
		}
		void add_to(obj& object, ecs::component_id index) {


		}
		void transfer_object(obj& object, ecs::component_id index) {

		
			Archtype* current = archtypes[object.meta().arch.id];
			if (current == nullptr)
			{
				throw std::logic_error("Component must be part of an archtype");
			}
			Option<archetype_id> id =  current->flip_id(index);
			if (!id)
			{
				stn::bitset new_arch = current->bit_list;

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
		stn::counter archtype_counter;
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
	using componentStorage = array<stn::flux_token<component>>;
	struct component_table
	{
		template<typename T>
		component_table(ecs::component_id mid, std::type_identity<T> type_identity):
			pool(stn::flux(std::type_identity<T>())),id(mid){
			store = array<stn::flux_token<component>>();
		}
		stn::memory::layout component_layout;
		stn::flux pool;
		componentStorage store;
		ecs::component_id id;
		component_description description;
		component* operator[](const obj& entity) {
			return store.reach(entity.id()).get_ptr();
		}
		const component* operator[](const obj& entity) const {
			return store[entity.id()].get_ptr();
		}
		bool contains(const obj& entity) const {
			return static_cast<bool>(store[entity.id()]);
		}
		void remove_at(const obj& entity) {
			store[entity.id()].release();
		}
		void set_at(const obj& entity,stn::flux_token<component>&& token) {
			store[entity.id()]=std::move(token);
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
		stn::bitset bitset_of() {
			array<size_t> indices = component_indexer.get_type_ids<Components>();
			stn::bitset res;
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
			component_indexer = type_map::type_indexer<ecs::component_id>();
			free_ids = array<uint32_t>(max_size);
			for (size_t i = 0; i < max_size; i++)
			{
				free_ids.push(i);

			}
			comp_storage = array<component_table>();

		}
		void inject_context(CtxName::Context* context)
		{
			int l = 1;
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
		std::tuple<Components*...> get_tuple(obj& obj, stn::array<ecs::component_id>& indices);
		template <class T, typename... types>
		T* add_component(obj& entity,types&&... initval);

		void updatecomponents(updatecalltype type);
		void delete_component(component* comp);
		template <class T, typename... types>
		stn::flux_token<component> create_component(obj& entity, types&&... initval);
		void bind_component(stn::flux_token<component>&& comp,obj& entity);
		type_map::type_indexer<ecs::component_id> component_indexer;
		Archtypes arch;
		Archtype& operator[](archetype_id index) {
			return arch[index];
		}
		array<component_table> comp_storage;
	private:
		component_table& component_table_with_id(ecs::component_id id) {
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
		Option<ecs::component_id> comp_id = OC->component_indexer.get_opt<T>();
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
	
	
		Option<ecs::component_id> comp_id = component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return nullptr;
		}
		component_table& complist = component_table_with_id(comp_id.unwrap());

		return (T*)(complist[object]);

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


		ecs::component_id comp_id = ecs::component_id(component_indexer.get<T>().id);

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
		Option<ecs::component_id> id = OC->component_indexer.get_opt<T>();
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
	inline void gameobject::Ecs::bind_component(stn::flux_token<component>&& comp, obj& entity)
	{
		
		comp.get_ptr()->owner = entity;
		component_table& row = component_table_with_id(comp.get_unchecked().comp_id);
		row.set_at(entity,std::move(comp));
		component* component_in_storage = row[entity];
	
		arch.transfer_object(entity, component_in_storage->comp_id);
		component_in_storage->start();
	}


	template<class T, typename ...Args>
	inline stn::flux_token<component> Ecs::create_component(obj& entity, Args&& ...args) 
	{
		auto [cmpid, is_new] = component_indexer.insert<T>();


		stn::flux_token<component> comp_token= stn::flux_token<component>();
		if (is_new)
		{
			component_table& row = comp_storage.emplace(cmpid, std::type_identity<T>());

			row.store.expand(entitymeta.cap());
			comp_token = row.pool.emplace_unchecked<T>(std::forward<Args>(args)...).abstract<component>();
			comp_token.get_ptr()->comp_id = cmpid;
			arch.expandArchtype();
			row.init(comp_token.get_ptr());

		}
		else
		{
			component_table& row = component_table_with_id(cmpid);
			if (row[entity] == nullptr)
			{
				comp_token = row.pool.emplace_unchecked<T>(std::forward<Args>(args)...).abstract<component>();
				comp_token.get_ptr()->comp_id = cmpid;

			}
		}
		

		return (comp_token);
	}
	template<class T, typename ...types>
	inline T* Ecs::add_component(obj& entity,types && ...initval)
	{ 
		int l = 1;
		static_assert(std::is_constructible_v<T, types...>, "no constructor for T takes in [types...]");
		stn::flux_token<component> comp =create_component<T,types...>(entity, std::forward<types>(initval)...);
		if (comp)
		{
			bind_component(std::move(comp), entity);
		}
		return getcomponentptr<T>(entity);
	}
	
	template <class T, typename... types>
	T* obj::addcomponent(types&&... initval)
	{
		return OC->add_component<T>(*this,std::forward<types>(initval)...);
	}

	
	inline bool gameobject::Archtype::has_component(ecs::component_id index)
	{
		
		if (bit_list.contains_index(index.id))
		{
			
			return bit_list[index.id];
		}
		return false;
	}
	inline bool Archtype::has_components(stn::bitset set) const
	{
		return (bit_list & set) == set;
	}
	inline bool Archtype::has_any(stn::bitset set) const
	{
		return bit_list.matches_any(set);
	}
	template<typename T>
	bool Archtype::has_component()
	{
		Option<ecs::component_id> comp_id = OC->component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return false;
		}
		return has_component(*comp_id);
		//will not care if component list has been changed during the iteration
	}
	
	//just trust this works
	template<typename... Components>
	std::tuple<Components*...> Ecs::get_tuple(obj& obj,stn::array<ecs::component_id>& indices) {
		

		return[&]<size_t... Is>(std::index_sequence<Is...>) {
			return std::tuple<Components*...>{
				(Components*)comp_storage[indices[Is].id][obj]...
			};
		}(std::index_sequence_for<Components...>{});
	}
}