#pragma once
namespace stn {
	template <typename T>
	struct change {
		change() :value(), cached() {
		}
		change(const T& val) :value(val), cached(val) {
		}
		change(T val) :value(val), cached(val) {
		}
		explicit operator const T& () const {
			return value;
		}
		explicit operator T& () {
			return value;
		}
		T& operator()() noexcept {
			return value;
		}
		const T& operator()() const {
			return value;
		}
		//sets both values
		void clear(const T& val) {
			value = val;
			cached = val;
		}
		bool changed() const {
			return value != cached;
		}
		void set(const T& val) {
			value = val;

		}
		void sync() {
			cached = value;
		}
		change(const change&) = default;
		change(change&&) = default;
		change& operator=(const change&) = default;
		change& operator=(change&&) = default;
		void revert() {
			value = cached;
		}
	private:
		T cached;
		T value;
	};
	struct dirty_flag {
		explicit dirty_flag(bool should_clean) noexcept :dirty(should_clean) {

		}
		dirty_flag(const dirty_flag&) = delete;
		dirty_flag& operator=(const dirty_flag&) = delete;
		dirty_flag& operator=(dirty_flag&&) = delete;
		dirty_flag(dirty_flag&& other) noexcept
			: dirty(other.is_dirty()) {
			other.mark_clean();
		}

		//returns if it has cleaned
		template<typename Func>
		bool clean(Func&& func) {
			if (dirty.exchange(false, std::memory_order_acquire)) {
				func();
				mark_clean();
				return true;
			}
			return false;
		}

		explicit operator bool() const noexcept {
			return is_dirty();
		}
		bool is_dirty() const noexcept {
			return dirty.load(std::memory_order_relaxed);
		}

		void mark_dirty() noexcept {
			dirty.store(true, std::memory_order_relaxed);
		}
	private:
		void mark_clean() noexcept {
			dirty.store(false, std::memory_order_relaxed);
		}
		std::atomic_bool dirty;

	};
	template<typename T>
	struct dirty_value {
		T value;
		dirty_flag* flag;
		const T* operator->() const {
			return &value;
		}
		const T& operator*() const {
			return value;
		}
		dirty_value& operator=(const T& new_value) {
			if (new_value != value) {
				value = new_value;
				flag->mark_dirty();
			}

		}
		bool operator==(const T& other) const{
			return value == other;
		}
		dirty_value(const T& v, dirty_flag& f)
			: value(v), flag(&f) {
		}

	};
}