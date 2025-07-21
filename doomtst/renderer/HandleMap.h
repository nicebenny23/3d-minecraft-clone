#include "../util/robinHood.h"
#include "../util/Id.h"
#include "../util/dynamicarray.h"
#include <string>
#pragma once
namespace handle {
	template <typename T>
	struct HandleMap {
		robin_hood::unordered_flat_map<std::string, size_t> string_to_id;
		stn::array<T> elemlist;
		Ids::Id get_handle(const std::string& Name) {

			if (!string_to_id.contains(Name))
			{
				string_to_id[Name] = string_to_id.size();
			}
			return Ids::Id(string_to_id[Name]);

		}
		const T& get_elem(Ids::Id handle) const{
			if (elemlist.length < handle.id)
			{
				throw std::out_of_range("Requested Shader does not exist");
			}
			return elemlist[handle.id];

		}
		bool Contains(const std::string& name) const{

			return string_to_id.contains(name);
		}
		void set(const std::string& Name,const T& value) {
			size_t handle = get_handle(Name).id;
			if (handle < elemlist.length)
			{
				throw std::logic_error("Two elements are unable to have the same name");
			}
			elemlist.push(value);
		}
	};

}