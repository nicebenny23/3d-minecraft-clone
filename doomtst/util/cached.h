#pragma once
namespace util {
	template <typename T>
	struct change {
		change() :value(), cached() {}
		change(const T& val) :value(val), cached(val) {}
		change(T val) :value(val), cached(val) {}
		explicit operator const T& () const noexcept { return value; }
		explicit operator T& () noexcept { return value; }
		T& operator()() noexcept { return value; }
		const T& operator()() const noexcept { return value; }
		//sets both values
		void reset(const T& val) {
			value = val;
			cached = val;
		}
		bool changed() {
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
}