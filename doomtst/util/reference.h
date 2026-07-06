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
		T* get_ptr() {
			return ptr;
		}
		const T* get_ptr() const {
			return ptr;
		}
		non_null() = delete;
		void observe(T& value) noexcept {
			ptr = &value;
		}
		non_null(T& value) :ptr(&value) {

		}
		non_null& operator=(T& value) {
			ptr = &value;
			return *this;
		}
		bool operator==(const T& value)  const requires std::equality_comparable<T> {
			return *ptr == value;
		}
		bool operator!=(const T& value) const requires std::equality_comparable<T> {
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