#pragma once
namespace stn {
	template <typename T>
	struct change {
		change() :value(), cached() {}
		change(const T& val) :value(val), cached(val) {}
		change(T val) :value(val), cached(val) {}
		explicit operator const T& () const { return value; }
		explicit operator T& () { return value; }
		T& operator()() noexcept { return value; }
		const T& operator()() const { return value; }
		//sets both values
		void clear(const T& val) {
			value = val;
			cached = val;
		}
		bool changed() const{
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
			: dirty(other.dirty) {
			other.dirty = false;
		}
		
		//returns if it has cleaned
		template<typename Func>
		bool clean(Func&& func) {
			if (dirty) {
				func();
				dirty = false;
				return true;
			}
			return false;
		}
		
		explicit operator bool() const noexcept {
			return dirty;
		}
		bool is_dirty() const noexcept {
			return dirty;
		}

		void mark_dirty() noexcept {
			dirty = true;
		} 
	private:
		bool dirty;

	};
}