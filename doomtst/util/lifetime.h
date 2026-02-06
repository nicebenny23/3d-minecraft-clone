#pragma once
#include "sharedptr.h"
#include "Option.h"
namespace stn {
	template<typename T>
	concept LifetimeEligible =
		std::is_object_v<T> &&                 
		!std::is_array_v<T> &&                 
		std::is_class_v<T> &&  
		std::destructible<T>&&
		std::is_standard_layout_v<T> &&        
		std::is_nothrow_move_constructible_v<T>; 

	template<typename T>
	concept LifetimeTrackable = stn::CalledWhileIncomplete<T>;
	template<LifetimeTrackable T>
	struct lifetime_state {
		lifetime_state(T& value) :ptr(&value) {
		}
		bool alive() const {
			return ptr;
		}
		void kill() {
			ptr = nullptr;
		}
		void move(T& new_location) {
			ptr = &new_location;
		}
		stn::Option<T&> operator*() {
			if (ptr) {
				return *ptr;
			}
			return stn::None;
		}
		stn::Option<const T&> operator*() const {
			if (ptr) {
				return *ptr;
			}
			return stn::None;
		}
	private:
		T* ptr;
	};
	template<LifetimeTrackable T>
	using lifetime_refrence = stn::rc<lifetime_state<T>>;
	template<LifetimeTrackable T>
	using lifetime_ptr = stn::rc_ptr<lifetime_state<T>>;

	template<LifetimeTrackable T>
	struct lifetime_tracker {
		lifetime_tracker(lifetime_refrence<T> lifetime) :value(lifetime) {

		}
		bool alive() const requires LifetimeTrackable<T> {
			return value->alive();
		}
		stn::Option<T&> operator*() requires LifetimeTrackable<T> {
			return **value;
		}
		stn::Option<const T&> operator*() const requires LifetimeTrackable<T> {
			return **value;
		}
	private:
		lifetime_refrence<T> value;
	};
	template<LifetimeTrackable T>
	struct lifetime_source {
		lifetime_source(T& object) :value(object),owner_delta(reinterpret_cast<std::byte*>(this)-reinterpret_cast<std::byte*>(&object)) {
		}
		T* real_owner_location(){
			return reinterpret_cast<T*>(reinterpret_cast<std::byte*>(this) - owner_delta);
		}
		~lifetime_source() {
			if (value) {
				value->kill();
			}
		}
	
		lifetime_source(lifetime_source&& other) :value(std::move(other.value)),owner_delta(std::move(other.owner_delta)) {
			if (value) {
				value->move(*real_owner_location());
			}
		}

		lifetime_source& operator= (lifetime_source&& other) {
			if (value) {
				value->kill();
			}
			value = std::move(other.value);
			if (value) {
				value->move(*real_owner_location());
			}
			return *this;
		}

		lifetime_tracker<T> view() const {
			return lifetime_tracker<T>(lifetime_refrence<T>::from_rc_ptr(value).expect("should exist"));
		}
	private:

		std::ptrdiff_t owner_delta;
		stn::lifetime_ptr<T> value;
	};


}