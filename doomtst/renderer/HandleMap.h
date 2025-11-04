#include "../util/robinHood.h"
#include "../util/Id.h"
#include "../util/dynamicarray.h"
#include <string>

#pragma once
namespace handle {
	template <typename Handle,typename T,typename HandleID=stn::Id>
	struct HandleMap {
		//maps the string to the internal id
		robin_hood::unordered_flat_map<Handle, uint32_t> handle_to_index;
		stn::array<stn::Option<T>> elems;
		//returns whether handle_id exists
		bool generated(const HandleID& handle_id) const {
			return elems.contains_index(handle_id.id);
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

		HandleID reserve(const Handle& handle) {
			if (generated(handle))
			{
				return HandleID(handle_to_index.at(handle));
			}
			uint32_t id = uint32_t(size());
				handle_to_index[handle] = id;
				elems.push(stn::None);
				return HandleID(id);
		}
		HandleID insert(const Handle& handle,const T& value) {
			if (generated(handle))
			{
				uint32_t id = handle_to_index[handle];
				elems[id] = value;
				return HandleID(id);

			}
			uint32_t id = uint32_t(size());
			handle_to_index[handle] = id;
			elems.push(value);
			return HandleID(id);
		}
	
		size_t size() const{
			return elems.size();
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
		stn::Option<const T&> operator[](HandleID handle) const {
			if (!generated(handle))
			{
				throw std::logic_error("handle is not contained in handle_map");
			}
			if (!bound(handle))
			{
				throw std::logic_error("handle has no element as it has not been bounded to a value");
			}
			return elems[handle.id].as_ref();
		}

		stn::Option<T&> operator[](HandleID handle_id) {
			if (!generated(handle_id))
			{
				throw std::logic_error("handle is not contained in handle_map");
			}
			if (!bound(handle_id))
			{
				throw std::logic_error("handle has no element as it has not been bounded to a value");
			}
			return elems[handle_id.id].as_ref();
		}
		
	};

}