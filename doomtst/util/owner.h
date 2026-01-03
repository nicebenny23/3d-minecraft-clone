#include "sharedptr.h"
#pragma once
//will delete once i get a proper renderer
namespace Cptr {
	template <typename T>
	struct cptr {

		stn::rc_ptr<bool> exists;
	private:
		T* pntr;
		//unsafe function

		//unsafe function
		void updatestate() {


			if (!exists|| !*exists) {
				pntr = nullptr;
			}
		}

	public:
		cptr() = default;
		cptr(const cptr<T>& other) = default;


		explicit cptr(T* p) {
			pntr = p;
			if (pntr != nullptr) {

				exists = stn::rc_ptr< bool>(new bool(true));
			}
			else {
				exists = stn::rc_ptr<bool>();
			}
		}

		// Template Assignment Operator for Derived Types
		template <typename U>
		void set(cptr<U>& other) {
			static_assert(std::is_base_of<T, U>::value, "U must be derived from T");
			exists = other.exists;
			pntr = other.ptr();
		}
		//safe
		bool valid() {
			updatestate();
			return (pntr != nullptr);


		}



		//safe
		T*& ptr() {
			updatestate();
			return pntr;
		};

		//safe
		void free() {
			updatestate();
			if (pntr == nullptr) {
				throw std::logic_error("cant delete a nullptr");
			}

			delete pntr;
			pntr = nullptr;
			if (exists.isValid()) {

				*exists = false;
				exists.free();
			}
		}


		T*& operator->() {

			return ptr();

		}
		//safe
		T& operator*() {
			return *ptr();
		}

		//safe
		bool operator==(T* other) {

			return ptr() == other;
		}
		//safe
		bool operator==(cptr<T>& other) {

			return ptr() == other.ptr();
		}
		bool operator!=(T* other) {

			return (ptr() != other);
		}
		//safe
		bool operator!=(cptr<T>& other) {

			return  (ptr() != other.ptr());
		}

		cptr(cptr&& other) noexcept = default;
		cptr& operator=(cptr&& other) noexcept = default;
	};
}