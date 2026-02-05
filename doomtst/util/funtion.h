#include <memory>
#pragma once
namespace stn {
	template <typename F, typename R, typename... Args>
	concept StatelessFunction =
		std::is_invocable_r_v<R, F, Args...> &&
		(std::is_pointer_v<std::remove_reference_t<F>> ||
			std::is_empty_v<std::remove_reference_t<F>>);
	template <typename>
	struct function_ref;

	template <typename R, typename... Args>
	struct function_ref<R(Args...)> {
		function_ref() = delete;
		using ptr_type = R(*)(Args...);
		//apperently the plus allows us to get it to work
		template <typename F>
		constexpr function_ref(F&& f) noexcept requires (StatelessFunction<F,R, Args...>)
			: fn(+f) {
		}

		template <typename F> requires (StatelessFunction<F,R, Args...>)
		constexpr function_ref& operator=(F&& f) noexcept {
			fn = +f;
			return *this;
		}
		constexpr R operator()(Args... args) const noexcept {
			return fn(args...);
		}
	private:
		ptr_type fn;
	};
}