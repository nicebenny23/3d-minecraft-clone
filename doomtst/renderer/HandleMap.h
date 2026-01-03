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
	template <typename Handle,typename T,typename HandleID=stn::Id>
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
			return handle_to_index.contains(handle) &&elems[handle_to_index.at(handle)].is_some();
		}

		//returns whether handle is reserved but not bound
		bool reserved(const HandleID& handle_id) const {
			return generated(handle_id)&&elems[handle_id.id].is_none();
		}
		//returns whether handle is reserved but not bound
		bool reserved(const Handle& handle) const {
			return generated(handle) && reserved(HandleID(handle_to_index.at(handle)));
		}
		
		stn::Option<const T&> operator[](HandleID handle) const {
			if (!generated(handle)) {
				return stn::None;
			}
			if (!bound(handle)) {
				return stn::None;
			}
			return elems[handle.id].as_ref();
		}

		stn::Option<T&> operator[](HandleID handle_id) {
			if (!generated(handle_id)) {
				return stn::None;
			}
			if (!bound(handle_id)) {
				return stn::None;
			}
			return elems[handle_id.id].as_ref();
		}

		HandleID reserve(const Handle& handle) {
			stn::insertion<uint32_t> insertion = insert_handle_index(handle);
			if (insertion.is_new) {

				elems.reach(insertion.value) = stn::None;
			}
			return HandleID(insertion.value);
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
			stn::insertion<uint32_t> insertion = insert_handle_index(handle);
			if (insertion.is_new || elems[insertion.value].is_none()) {
				elems.reach(insertion.value) = func(handle, HandleID(insertion.value));
			}
			return stn::Option<HandleID>(insertion.value).filter([this](HandleID id) {return elems[id.id].is_some(); });
		}

		HandleID get_or(const Handle& handle, const T& value) {
			stn::insertion<uint32_t> insertion = insert_handle_index(handle);
			if (insertion.is_new || elems[insertion.value].is_none()) {
				elems.reach(insertion.value) = value;
			}
			return HandleID(insertion.value);
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
			return (*this)[id_of(handle)].unwrap();
		}
		T& element_of(const Handle& handle) {
			return (*this)[id_of(handle)].unwrap();
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
			stn::insertion<uint32_t> insert_handle_index(const Handle& handle) {
				auto it = handle_to_index.find(handle);
				if (it == handle_to_index.end()) {
					uint32_t index = next_index();
					handle_to_index[handle] = index;
					index_to_handle.emplace(index, handle);
					return stn::insertion<uint32_t>(index, true);
				}
				return stn::insertion<uint32_t>(it->second, false);
			}
			uint32_t next_index() {
				if (free_ids.nonempty()) {
					return free_ids.pop();
				}
				else {
					return elems.length();
				}
			}
		
	};

}