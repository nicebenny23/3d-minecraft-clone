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
	struct ArchetypeIndexTag {};
	using archetype_index =stn::typed_index<ArchetypeIndexTag>;
	//index of a distint archetypr
	struct Archetype_tag {};
	using archetype_id = stn::typed_id<Archetype_tag>;
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
		T& addcomponent(types&&... initval);

		inline EntityMetadata& meta();
	
		constexpr obj() noexcept :entity() {
			
			OC = nullptr;
		}
		void destroy();
		

		Transform& transform();
		
		Ecs* OC;
	private:
		friend bool operator==(const obj& lhs, const obj& rhs) {
			return lhs.entity == rhs.entity;
		}
		friend bool operator!=(const obj& lhs, const obj& rhs) {
			return lhs.entity != rhs.entity;
		}
		ecs::space_id entity;
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
	template<typename T>
	concept ComponentType = std::derived_from<T, component>;

	struct Archetype {
		stn::bitset bit_list;
		array<ecs::component_id> positions;
		array<obj> elems;
		array<Option<archetype_id>> moves;
		template <ComponentType T>
		bool has_component();
		bool has_component(ecs::component_id ind);
		bool has_components(stn::bitset set) const;
		bool has_any(stn::bitset set) const;
		
		void add(obj& object) {
			elems.push(object);
		
			EntityMetadata& met = object.meta();
			met.arch=Id;
			met.arch_ind = archetype_index(std::uint32_t(elems.length() - 1));
		}
		void remove(obj& object) {
		
			if (elems.empty()) {
				throw std::logic_error("Cannot remove an entity from an empty archetype");
			}
			EntityMetadata& met = object.meta();
			if (Id!= met.arch) {
				throw std::logic_error("Cannot remove an entity from an archetype it does not reside in");
			}
			std::uint32_t index = met.arch_ind.index;
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

		Archetype(stn::bitset st, Ecs* Man,archetype_id id) :bit_list(st), OC(Man), elems(),Id(id) {

			for (size_t ind = 0; ind < st.bits; ind++) {
				moves.push(stn::None);

			}
			positions = st.indices<ecs::component_id>();
		};
		obj& operator[](std::uint32_t arch_index) {
			return elems[arch_index];
		}
		const obj& operator[](std::uint32_t arch_index) const {
			return elems[arch_index];
		}

		std::uint32_t count() const{
			return std::uint32_t(elems.length());
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


	
	// A list of Archetypes
	struct Archetypes {
		Archetypes() {
			OC = nullptr;
		}
		Archetypes(Ecs* man) :OC(man) {
			addArchetype(stn::bitset());
		};
		inline Archetype& operator[](size_t index) {
			return *archetypes[index];
		}
		inline const Archetype& operator[](size_t index) const{
			return *archetypes[index];
		}
		Archetype& operator[](archetype_id Id) {
			return *archetypes[Id.id];
		}
		const Archetype& operator[](archetype_id Id) const{
			return *archetypes[Id.id];
		}
		
		inline size_t length() const{
			return archetypes.length();
		}
		const Archetype& empty_archetype() const{
			return *archetypes[0];
		}
		Archetype& empty_archetype() {
			return *archetypes[0];
		}
		void expandArchetype() {

			for (auto& arch : archetypes)
			{
				arch->bit_list.expand(false);
				arch->moves.expand(arch->moves.length() + 1);
			}

		}
		
		void addArchetype(const stn::bitset& Components) {

		
			for (Archetype* arch : archetypes) {
				if (arch->bit_list.ones_match(Components)) {
					throw std::logic_error("ecs cannot contain duplicate archetypes");
				}
			}
			archetypes.push(new Archetype(Components, OC,archetype_id(std::uint32_t(archetypes.length()))));

			for (auto& arch : archetypes)
			{
				stn::bitset archxor = Components ^ arch->bit_list;
				if (archxor.popcount() == 1)
				{
					for (size_t j = 0; j < archxor.bits; j++)
					{
						if (archxor[j])
						{

							archetypes.last()->moves[j] = archetype_id(arch->id());
							arch->moves[j] = archetype_id(archetypes.last()->id());
						}
					}
				}
			}

			
		}
		void drop(obj& object) {

			EntityMetadata e_meta= object.meta();
			archetype_index index = e_meta.arch_ind;
			Archetype& stored = *archetypes[e_meta.arch.id];
			stored[stored.count() - 1].meta().arch_ind= index;
			stored.remove(index);
		}
		
		void transfer_object(obj& object, ecs::component_id index) {

			Archetype* current = archetypes[object.meta().arch.id];
			if (current == nullptr)
			{
				throw std::logic_error("Component must be part of an archetype");
			}
			archetype_id id = current->flip_id(index).unwrap_or_else([&]() {
				stn::bitset new_arch = current->bit_list;
				new_arch.reaching_flip(index.id);
				addArchetype(new_arch);
				//since it now exists
				return 	current->flip_id(index).unwrap();
				});
			current->remove(object);
			archetypes[id.id]->add(object);


		}

		using iterator = typename stn::array<Archetype*>::iterator;
		iterator begin() { return archetypes.begin(); }
		iterator end() { return archetypes.end(); }

	private:
		array<Archetype*> archetypes;
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
		component* checked_get(const obj& entity) {
			if (!contains(entity))
			{
				stn::throw_logic_error("component_table did not contain entity with id {}", entity.id());
			}
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

			if (entity.id() == 520191&&id.id==0)
			{
				int l = 3;
			}
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
			array<size_t> indices = component_indexer.get_ids<Components>();
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
			arch = Archetypes(this);
			const size_t max_size = static_cast<size_t>(1) << 19;
			ctx = nullptr;
			entitymeta = array<EntityMetadata>(max_size);
			component_indexer = type_map::type_indexer<ecs::component_id>();
			free_ids = array<uint32_t>(max_size);
			for (size_t i = 0; i < max_size; i++)
			{
				free_ids.push(uint32_t(i));

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

		template<ComponentType T>
		T* getcomponentptr(obj object);
		template<ComponentType T>
		T& getcomponent(obj object);
		obj spawn_with_transform(v3::Vec3 SpawnPos);
		obj spawn_empty();

		void destroy(obj& object);

		template<ComponentType... Components>
		std::tuple<Components*...> get_tuple(obj& obj, stn::array<ecs::component_id>& indices);
		template <ComponentType T, typename... types>
		T& add_component(obj& entity,types&&... initval);

		void updatecomponents(updatecalltype type);
		void delete_component(component* comp);
		template <ComponentType T, typename... types>
		stn::flux_token<component> create_component(obj& entity, types&&... initval);
		void bind_component(stn::flux_token<component>&& comp,obj& entity);
		type_map::type_indexer<ecs::component_id> component_indexer;
		Archetypes arch;
		Archetype& operator[](archetype_id index) {
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
	
	

	struct StaticComponent : gameobject::component
	{


	};
	struct transform_comp : gameobject::component {
		Transform transform;

	};





	template <class T>
	void obj::removecomponent()
	{
	
		if (!hascomponent<T>()) {
			return;
		}
		Option<ecs::component_id> comp_id = OC->component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return;
		}
		ecs::component_id id = comp_id.unwrap();
		OC->arch.transfer_object(*this, id);
		if (!OC->comp_storage[id.id].contains(*this)) {
			throw std::logic_error("invariant violation:component must exist if id is contained");
		}
		OC->comp_storage[id.id][*this]->destroy();

			
		

	}


	template<ComponentType T>
	T* Ecs::getcomponentptr(obj object)
	{
	
		Option<ecs::component_id> comp_id = component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return nullptr;
		}
		component_table& complist = component_table_with_id(comp_id.unwrap());

		return (T*)(complist[object]);

	}
	template<typename T>
	inline T* obj::getcomponentptr()
	{
		if (!OC)
		{
			throw std::logic_error("Object Not initilized");
		}
		return OC->getcomponentptr<T>(*this);
	}

	template<ComponentType T>
	T& Ecs::getcomponent(obj object)
	{

		ecs::component_id comp_id = component_indexer.get<T>();
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
		return OC->getcomponent<T>(*this);
	}
	
	template <class T>
	inline bool obj::hascomponent() 
	{
		return OC->component_indexer.get_opt<T>().is_some_and([&](auto&& id) {return (*OC)[meta().arch].has_component(id); });
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


	template<ComponentType T, typename ...Args>
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
			arch.expandArchetype();
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
	template<ComponentType T, typename ...types>
	inline T& Ecs::add_component(obj& entity,types && ...initval)
	{ 
		int l = 1;
		static_assert(std::is_constructible_v<T, types...>, "no constructor for T takes in [types...]");
		stn::flux_token<component> comp =create_component<T,types...>(entity, std::forward<types>(initval)...);
		if (comp)
		{
			T* component = comp.get_unchecked_ptr<T>();
			bind_component(std::move(comp), entity);
			return *component;
		}
		return getcomponent<T>(entity);
	}
	
	template <class T, typename... types>
	T& obj::addcomponent(types&&... initval)
	{
		return OC->add_component<T>(*this,std::forward<types>(initval)...);
	}

	
	inline bool gameobject::Archetype::has_component(ecs::component_id index)
	{
		
		if (bit_list.contains_index(index.id))
		{
			
			return bit_list[index.id];
		}
		return false;
	}
	inline bool Archetype::has_components(stn::bitset set) const
	{
		return bit_list.ones_match(bit_list|set);
	}
	inline bool Archetype::has_any(stn::bitset set) const
	{
		return bit_list.matches_any(set);
	}
	template<ComponentType T>
	bool Archetype::has_component()
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
	template<ComponentType... Components>
	std::tuple<Components*...> Ecs::get_tuple(obj& obj,stn::array<ecs::component_id>& indices) {
		
		return[&]<size_t... Is>(std::index_sequence<Is...>) {
			return std::tuple<Components*...>{
				(Components*)comp_storage[indices[Is].id][obj]...
			};
		}(std::index_sequence_for<Components...>{});
	}
}                                    