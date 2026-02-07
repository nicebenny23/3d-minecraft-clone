#include "Option.h"
#pragma once
namespace stn {
    template<typename Sig>
    struct select_overload_t;

    // Partial specialization for any R(Args...)
    template<typename R, typename... Args>
    struct select_overload_t<R(Args...)> {
        // free functions
  //      consteval bool has_free_overload(R(*f)(Args...)) const noexcept { return true; }

        // member functions: non-const
        template<typename C>
        consteval auto has_free_overload(R(C::* f)(Args...)) const noexcept { return true; }

        // member functions: const
        template<typename C>
        consteval auto has_free_overload(R(C::* f)(Args...) const) const noexcept { return true; }

        // instance of functor: non-const operator()
        template<typename F>
        static constexpr bool has_functor_overload() {
            using T = std::remove_cvref_t<F>;
            if constexpr (requires { static_cast<R(T::*)(Args...)>(&T::operator()); }) {
                return true;
            }
            else if constexpr (requires { static_cast<R(T::*)(Args...) const>(&T::operator()); }) {
                return true;
            }
            return false;

        }


    };
    template<typename F, typename Sig>
	constexpr bool has_exact_overload() {
		return stn::select_overload_t<Sig>().template has_functor_overload<F>();

	}
}