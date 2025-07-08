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
#include "../util/type_index.h"
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
namespace gameobject {

	



	enum updatecalltype {
		Framecall = 0,
		Rendercall = 1,


	};
	struct component;
	struct OCManager;
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
	//using ArchStorage = Sparse::SparseSet < size_t>;
	struct ComponentMapper {
		constexpr size_t operator()(component* comp) const noexcept;



	};
	
	struct EntityMetadata {
	

		Archtype* arch;
		objstate state;
		size_t gen_count;
		size_t arch_ind;
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


		objstate& state();

		constexpr obj() noexcept {
			Id = Ids::None;
			OC = nullptr;
		}

		void immediate_destroy();

		void deffered_destroy();

		Transform& transform();

		OCManager* OC;
	private:



		friend struct OCManager;
	};
	static constexpr obj None = obj();

	struct Archtype {
		array<Archtype*> moves;
		array<obj> elems;
		array<size_t> dense_bits;
		bitset::bitset bit_list;
		template <typename T>
		bool has_component();
		bool has_component(size_t index);
		void add(obj object) {
			elems.push(object);
			object.meta().arch = this;
			object.meta().arch_ind = elems.length - 1;
		}
		void remove(obj object) {
			
			EntityMetadata& met= object.meta();
			if (this!=met.arch)
			{
				throw std::logic_error("cannot remove a componet from an archtype it is not a member of");
			}
			size_t index = met.arch_ind;
			size_t end_index = elems.length - 1;
			std::swap(met.arch_ind, elems[end_index].meta().arch_ind);
			std::swap(elems[index], elems[end_index]);
			met.arch = nullptr;
			elems.pop();
		}
		//Sparse::SparseSet<size_t> proper_inds;
		OCManager* OC;
		Archtype(bitset::bitset st,OCManager* Man) :bit_list(st), OC(Man) {
			for (size_t ind = 0; ind < st.bits; ind++) {
				if (bit_list[ind]) {
					dense_bits.push(ind);
				}
			}
		};
		Archtype() {
			OC = nullptr;
			
			bit_list = bitset::None;
		}
	};


	using componentStorage = Sparse::SparseSet< component*, ComponentMapper>;
	struct componentmanager
	{
		componentmanager();
		void create(int mid, int bytesize);
		updatetype utype;
		int priority;
		size_t id;
	
		dynPool::flux<component> pool;
		componentStorage store;
		
		void init(component* sample);
	};
	
	

	struct OCManager {
	
		OCManager(){
			const size_t max_size = static_cast<size_t>(1) << 20;
			ctx = nullptr;
			entitymeta = array<EntityMetadata>(max_size);
			free_ids = array<size_t>(max_size);
			for (size_t i = 0; i < max_size; i++)
			{
				free_ids.push(i);
				
			}
			managers = array<componentmanager, true>(0);
			archtypes.push(Archtype(bitset::None,this));
			archtypes[0].bit_list.push(false);
		}
		void inject_context(CtxName::Context* context)
		{
			ctx = context;
		}
		array<EntityMetadata> entitymeta;
		
	
		
		obj CreateEntity(v3::Vector3 SpawnPos);
		void InitObj(obj* object);

		void destroy(obj* object);
		
		
		  template <class T, typename... types>
		  T* InitComp(types&&... initval);

		  void updatecomponents(updatecalltype type);

		 void Delete_deffered_objs();

		 void delete_component(component* comp);

		 Cont::stack<obj> EntityDeletionBuffer;
		 array<Archtype> archtypes;
		 void expandArchtype() {
			
				 for (auto& arch : archtypes)
				 {
					 arch.bit_list.push(false);
				 }
			 
		 }

		 void addArchtype(bitset::bitset Components) {
			
			 archtypes.push(Archtype(Components,this));
			 for (auto& arch:archtypes)
			 {
				 bitset::bitset archxor = Components ^ arch.bit_list;
				 if (archxor.popcount() == 1)
				 {
					 for (size_t j = 0; j < archxor.bits; j++)
					 {
						 if (archxor[j])
						 {
							 archtypes[archtypes.length - 1].moves[j] = &arch;
							 arch.moves[j] = &archtypes[archtypes.length - 1];
						 }
					 }
				 }
			 }
		 }
		 type_id::dense_type_system comp_map;
		 void moveflipArch(obj object, size_t index) {
			 Archtype* current = object.meta().arch;
			 if (current == nullptr)
			 {
				 throw std::logic_error("Component must be part of an archtype");
			 }
			 
			 Archtype* new_type = current->moves[index];
			 if (new_type==nullptr)
			 {
				 bitset::bitset new_arch = current->bit_list;
				 new_arch.flip(index);
				 addArchtype(new_arch);
				 new_type = current->moves[index];
			 }
			 current->remove(object);
			 new_type->add(object);
		 }

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
		public:

			obj owner;

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
		struct transform_comp : gameobject::component {
			Transform transform;

		};

		
	

	
		template <class T>
		void obj::removecomponent()
		{
			//replace with a get components call
			verify_component<T>();

			size_t comp_id = OC->comp_map.get<T>();

			componentStorage& complist = OC->managers[comp_id].store;
			if (complist.contains_key(Id.id))
			{
				OC->moveflipArch(*this, comp_id);
				component* comp = complist.getByKey(Id.id);
				if (comp != nullptr) {
					comp->destroy();
				}
			}
			
		}


		void destroy(obj* object);

		template<class T>
		T* obj::getcomponentptr()
		{
			verify_component<T>();

			size_t comp_id = OC->comp_map.get<T>();	
			componentStorage& complist = OC->managers[comp_id].store;
				return (T*)(complist.getByKey(Id.id	));
		
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
			size_t id = OC->comp_map.get<T>();
			return meta().arch->has_component(id);
			
		}






		template <class T, typename... types>


		T* obj::addcomponent(types&&... initval)
		{

			verify_component<T>();

			//transfer this section over to the Update Manager
			T* comp = OC->InitComp<T>(std::forward<types>(initval)...);
		

			OC->moveflipArch(*this, comp->comp_id);
			comp->owner = *this;

			componentStorage& complist = OC->managers[comp->comp_id].store;
			complist.push(comp);
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
				expandArchtype();
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

			return obj1.Id == obj2.Id;
		}
		inline bool gameobject::Archtype::has_component(size_t index)
		{
			return bit_list.at(index);
		}
		template<typename T>
		bool Archtype::has_component()
		{
		return has_component(OC->comp_map.get<T>());
		}


		struct ComponentIterator {
			obj owner;
			OCManager* manager;
			const array<size_t>* dense_bits;
			size_t pos = 0;

			ComponentIterator(obj o, OCManager* m, const array<size_t>* bits, size_t p = 0)
				: owner(o), manager(m), dense_bits(bits), pos(p) {
			}

			component* operator*() {
				size_t comp_id = (*dense_bits)[pos];
				auto& store = manager->managers[comp_id].store;
				if (!store.contains_key(owner.Id.id)) {
					// FAIL FAST with a clear error
					throw std::logic_error(
						"ECS invariant violated: archetype lists comp_id " +
						std::to_string(comp_id) +
						" for entity " + std::to_string(owner.Id.id) +
						", but store does not contain that key."
					);
				}
				return store.getByKey(owner.Id.id);
			}
			ComponentIterator& operator++() {
				++pos;
				return *this;
			}

			bool operator!=(const ComponentIterator& other) const {
				return pos != other.pos;
			}
		};

		struct ComponentView {
			obj owner;

			ComponentView(obj o) : owner(o) {}

			ComponentIterator begin() {
				auto* bits = &owner.meta().arch->dense_bits;
				return ComponentIterator(owner, owner.OC, bits, 0);
			}

			ComponentIterator end() {
				auto* bits = &owner.meta().arch->dense_bits;
				return ComponentIterator(owner, owner.OC, bits, bits->length);
			}
		};
}

