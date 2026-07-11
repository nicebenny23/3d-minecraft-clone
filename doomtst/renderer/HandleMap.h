#include "../util/robinHood.h"
#include "../util/Id.h"
#include "../util/dynamicarray.h"
#include <string>

#pragma once
namespace handle {
	template<typename T>
	concept Mappable = requires(const T & a) {
		{
			std::hash<T>{}(a)
		} -> std::convertible_to<std::size_t>;
	}
	&& std::equality_comparable<T>;
	template <typename Handle,typename T,typename HandleID>
	struct HandleMap {
		//maps the string to the internal id
		robin_hood::unordered_flat_map<Handle, uint32_t> handle_to_index;
		stn::array<stn::Option<T>> elems;
		robin_hood::unordered_flat_map<uint32_t,Handle> index_to_handle;
		stn::array< uint32_t> free_ids;
		//returns whether handle_id exists
		bool generated(const HandleID& handle_id) const {
			return index_to_handle.contains(handle_id.id);
		}
		//returns whether handle exists
		bool generated(const Handle& handle) const {
			return handle_to_index.contains(handle);
		}

		//returns whether handle has an element assosiated with it 

		bool bound(const HandleID& handle_id) const {
			return generated(handle_id)&&elems[handle_id.id].is_some();
		}

		//returns whether handle_id has an element assosiated with it 
		bool bound(const Handle& handle) const {
			return generated(handle) &&elems[handle_to_index.at(handle)].is_some();
		}

		//returns whether handle is reserved but not bound
		bool reserved(const HandleID& handle_id) const {
			return generated(handle_id)&&elems[handle_id.id].is_none();
		}
		//returns whether handle is reserved but not bound
		bool reserved(const Handle& handle) const {
			return generated(handle) && reserved(HandleID(handle_to_index.at(handle)));
		}
		
		stn::Option<const T&> operator[](HandleID handle_id) const {
			return elems.get_flat_ref(handle_id.id);
		}

		stn::Option<T&> operator[](HandleID handle_id) {
			return elems.get_flat_ref(handle_id.id);
		}

		HandleID reserve(const Handle& handle) {
			stn::Insertion<uint32_t> Insertion = insert_handle_index(handle);
			if (Insertion.is_new) {

				elems.reach(Insertion.value) = stn::None;
			}
			return HandleID(Insertion.value);
		}
		HandleID insert(const Handle& handle,const T& value) {
			uint32_t id = insert_handle_index(handle).value;
			elems.reach(id) = value;
			return HandleID(id);
		}
		template<typename Func>
			requires std::invocable<Func, const Handle&, const HandleID&>&&
		std::same_as<std::invoke_result_t<Func, const Handle&, const HandleID&>, stn::Option<T>>
			stn::Option<HandleID> get_or_try(const Handle& handle, Func&& func) {
			stn::Insertion<uint32_t> Insertion = insert_handle_index(handle);
			if (Insertion.is_new || elems[Insertion.value].is_none()) {
				elems.reach(Insertion.value) = func(handle, HandleID(Insertion.value));
			}
			return stn::Option<HandleID>(Insertion.value).filter([this](HandleID id) {return elems[id.id].is_some(); });
		}

		HandleID get_or(const Handle& handle, const T& value) {
			stn::Insertion<uint32_t> Insertion = insert_handle_index(handle);
			if (Insertion.is_new || elems[Insertion.value].is_none()) {
				elems.reach(Insertion.value) = value;
			}
			return HandleID(Insertion.value);
		}

		//gets the handle of a generated element 
		HandleID id_of(const Handle& handle) const{
			if (!generated(handle))
			{
				throw std::logic_error("handle is not contained in handle_map");
			}
			return HandleID(handle_to_index.at(handle));
		}
		
		const T& element_of(const Handle& handle) const{
			return elems[id_of(handle).id].expect(" handle must be bound");
		}
		T& element_of(const Handle& handle) {
			return elems[id_of(handle).id].expect(" handle must be bound");
		}
		T& element_of(const HandleID& handle_id) {
			return (*this)[handle_id].unwrap();
		}
		const T& element_of(const HandleID& handle_id) const{
			return (*this)[handle_id].unwrap();
		}

		void remove_handle(const Handle& handle) {
			auto it = handle_to_index.find(handle);
			if (it != handle_to_index.end()) {
				uint32_t index = it->second;
				handle_to_index.erase(it); // good
				elems[index] = stn::None;
				index_to_handle.erase(index);
				free_ids.push(index);
			}


		}
		void remove_id(HandleID Id) {
			auto it = index_to_handle.find(Id.id);
			if (it != index_to_handle.end()) {
				uint32_t index = Id.id;
				handle_to_index.erase(it->second);
				elems[index] = stn::None;
				index_to_handle.erase(it);
				free_ids.push(index);
			}
		}
		void clear() {
			handle_to_index.clear();
			index_to_handle.clear();
			elems.clear();
			free_ids.clear();
		}
		private:
			stn::Insertion<uint32_t> insert_handle_index(const Handle& handle) {
				auto it = handle_to_index.find(handle);
				if (it == handle_to_index.end()) {
					uint32_t index = next_index();
					handle_to_index[handle] = index;
					index_to_handle.emplace(index, handle);
					return stn::Insertion<uint32_t>(index, true);
				}
				return stn::Insertion<uint32_t>(it->second, false);
			}
			uint32_t next_index() {
				if (free_ids.non_empty()) {
					return free_ids.pop();
				}
				else {
					return static_cast<uint32_t>(elems.length());
				}
			}
		
	};
	struct handle {

	};

	template<typename T>
	struct assets {
	
		handle load() {

		}
	};

}