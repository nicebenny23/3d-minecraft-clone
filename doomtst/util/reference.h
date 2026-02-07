#pragma once
namespace stn {

	template<typename T>
	struct non_null {
		T& get() {
			return *ptr;
		};
		const T& get() const {
			return *ptr;
		};
		T& operator*() {
			return *ptr;
		};
		const T& operator*() const {
			return *ptr;
		};
		non_null() = delete;
		void assign_value(const T& value) noexcept {
			*ptr = value;
		}
		non_null(T& value) :ptr(&value) {

		}
		non_null& operator=(T& value) {
			ptr = &value;
			return *this;
		}
		bool operator==(const T& value) const {
			return *ptr == value;
		}
		bool operator!=(const T& value) const {
			return *ptr != value;
		}
		T* operator->() {
			return ptr;
		}
		const T* operator->() const {
			return ptr;
		}
	private:
		T* ptr;
	};





}