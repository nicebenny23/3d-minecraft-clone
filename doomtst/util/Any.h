#include "type_index.h"
#pragma once
namespace stn {

	struct AnyRef {
		bool operator==(const AnyRef& other) const {
			if (id_for == other.id_for) {
				return id_for.equals().expect("Refrences must be comparable")(data, other.data);
			}
			return false;
		}
		size_t hash() {
			return id_for.hash().expect("Refrence must be hashable")(data);
		}
		type_id id() const{
			return id_for;
		}
		static AnyRef make_unchecked(void* ptr, type_id id) {
			return AnyRef(ptr, id);
		}
	private:
		AnyRef(void* ptr, type_id object_id) :data(ptr), id_for(object_id) {

		}

		void* data;
		type_id id_for;

	};
}