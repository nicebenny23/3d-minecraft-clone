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
#include "../util/type_index.h"
#include "Commands.h"
#include "Event.h"
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

		inline bool exists() const;

		Ids::GenId GenId;

		template <class T, typename... types>
		T* addcomponent(types&&... initval);

		inline EntityMetadata& meta();


		constexpr obj() noexcept {
			GenId = Ids::NoneG;
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
		bool has_any(bitset::bitset set);
		void add(obj& object) {
			EntityMetadata& met = object.meta();
			if (met.arch != nullptr)
			{
				throw std::logic_error("Cannot add entity already in a Archytype to an archtype");
			}
			elems.push(object);
			met.arch = this;
			met.arch_ind = elems.length - 1;
		}
		void remove(obj& object) {
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
		}
	
		Archtype(bitset::bitset st, Ecs* Man) :bit_list(st), OC(Man), elems() {

			for (size_t ind = 0; ind < st.bits; ind++) {
				if (bit_list[ind]) {
					dense_bits.push(ind);
				}

				moves.push(nullptr);
			}
		};
		Archtype():moves() {
			OC = nullptr;
			
			bit_list = bitset::bitset();
		}


		using iterator = typename stn::array<obj>::iterator;

		iterator begin() { return elems.begin(); }
		iterator end() { return elems.end(); }

	};


	using componentStorage = array<component*>;
	struct component_table
	{
		component_table();
		void create(comp::Id mid, size_t bytesize, size_t alignment);
		comp::Id id;
		dynPool::flux<component> pool;
		componentStorage store;
		updatetype utype;
		int priority;
		component*& operator[](const obj& entity) {
			return store.reach(entity.GenId.id);
		}
		void init(component* sample);
	};


	struct ArchtypeManager {
		ArchtypeManager() {
			OC = nullptr;
		}
		ArchtypeManager(Ecs* man) :OC(man) {
			archtypes.push(new Archtype(bitset::bitset(), OC));
		};
		bool check() {
			for (Archtype* arch : archtypes) {
				if (!arch) continue;
				if (arch->elems.length == 1 && arch->elems[0].meta().arch != arch) {
					return true;
				}
			}
			return false;
		}
		inline Archtype& operator[](size_t index) {
			return *archtypes[index];
		}
		inline size_t length() {
			return archtypes.length;
		}
		array<Archtype*> archtypes;
		void expandArchtype() {

			for (auto& arch : archtypes)
			{
				arch->bit_list.push(false);
				arch->moves.expand(arch->moves.length + 1);
			}

		}

		void addArchtype(const bitset::bitset& Components) {

		
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

			
		}

		void moveflipArch(obj& object, comp::Id index) {

		
			Archtype* current = object.meta().arch;
			if (current == nullptr)
			{
				throw std::logic_error("Component must be part of an archtype");
			}
			Archtype* next = current->moves[index.id];
			if (next== nullptr)
			{
				bitset::bitset new_arch = current->bit_list;

				new_arch.flip(index.id);
				addArchtype(new_arch);
				//since it now exists
				next = current->moves[index.id];
				if (next == nullptr)
				{
					throw std::logic_error("Improper archytype allocation");
				}

			}
		
		
			current->remove(object);
			next->add(object);


		}

		using iterator = typename stn::array<Archtype*>::iterator;
		iterator begin() { return archtypes.begin(); }
		iterator end() { return archtypes.end(); }
	private:
		Ecs* OC;
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
			arch = ArchtypeManager(this);
			const size_t max_size = static_cast<size_t>(1) << 19;
			ctx = nullptr;
			entitymeta = array<EntityMetadata>(max_size);

			free_ids = array<size_t>();
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

		template<typename T>
		T* getcomponentptr(obj& object);
		template<typename T>
		T& getcomponent(obj& object);
		obj CreateEntity(v3::Vec3 SpawnPos);
		void InitializeEntity(obj& object);

		void destroy(obj& object);

		template<typename... Components>
		std::tuple<Components*...> get_tuple(obj& obj, stn::array<comp::Id>& indices);
		template <class T, typename... types>
		T* add_component(obj& entity,types&&... initval);

		void updatecomponents(updatecalltype type);
		void delete_component(component* comp);


		type_id::type_indexer component_indexer;
		ArchtypeManager arch;
		array<component_table> comp_storage;
	private:

		CtxName::Context* ctx;
		array<size_t> free_ids;
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
	struct component
	{
		comp::Id comp_id;
		obj owner;
		short priority;
		updatetype utype;
		//called on destroy used for deallocation

		virtual void destroy_hook();
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

		Opt::Option<comp::Id> comp_id = OC->component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return;
		}

		auto& complist = OC->comp_storage[(*comp_id).id].store;
		
		if (!hascomponent<T>()) {
			return;
		}
		OC->arch.moveflipArch(*this, *comp_id);
			
		
		component* comp = complist[GenId.id];
		if (!comp) {
			throw std::logic_error("invarient violation:component must exist if id is contained");
		}

				comp->destroy();

			
		

	}


	template<class T>
	T* Ecs::getcomponentptr(obj& object)
	{
		verify_component<T>();
	
	
		Opt::Option<comp::Id> comp_id = component_indexer.get_opt<T>();
		if (!comp_id)
		{
			return nullptr;
		}
		componentStorage& complist = comp_storage[comp_id.unwrap().id].store;

		return (T*)(complist[object.GenId.id]);

	}
	template<class T>
	inline T* obj::getcomponentptr()
	{
		return OC->getcomponentptr<T>(*this);
	}

	template<class T>
	T& Ecs::getcomponent(obj& object)
	{
		verify_component<T>();


		comp::Id comp_id = component_indexer.get<T>();

		component* comp= comp_storage[comp_id.id][object];
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
		verify_component<T>();
		Opt::Option<comp::Id> id = OC->component_indexer.get_opt<T>();
		if (!id)
		{
			return false;
		}
		return meta().arch->has_component(*id);

	}



	template<class T, typename ...types>
	inline T* Ecs::add_component(obj& entity,types && ...initval)
	{
		auto [cmpid, is_new] = component_indexer.insert<T>();
	

		component_table& row = comp_storage.reach(cmpid.id);
			if (row[entity] != nullptr)
			{
				return (T*)row[entity];
			}
			if (is_new)
			{
				row.create(cmpid, sizeof(T), alignof(T));
				row.store.expand(entitymeta.capacity);
			
			}
			T* comp = new (row.pool.alloc()) T(std::forward<types>(initval)...);
			comp->comp_id = cmpid;
		
			comp->owner = entity;
			row[entity] = (component*)(comp);
		if (is_new)
		{
			arch.expandArchtype();
			row.init(comp);
		}
		arch.moveflipArch(entity, cmpid);

		comp->start();
		return (T*)comp;

	}

	template <class T, typename... types>
	T* obj::addcomponent(types&&... initval)
	{
		return OC->add_component<T>(*this,std::forward<types>(initval)...);
	}

	inline bool operator == (const obj& obj1, const obj& obj2) {
		return obj1.GenId == obj2.GenId;
	}
	inline bool gameobject::Archtype::has_component(comp::Id index)
	{
		if (index.id < bit_list.bits)
		{
			return bit_list.at(index.id);
		}
		return false;
	}
	inline bool Archtype::has_components(bitset::bitset set)
	{
		return (bit_list & set) == set;
	}
	inline bool Archtype::has_any(bitset::bitset set)
	{
		return (bit_list & set).popcount()!=0;
	}
	template<typename T>
	bool Archtype::has_component()
	{
		Opt::Option<comp::Id> comp_id = OC->component_indexer.get_opt<T>();
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