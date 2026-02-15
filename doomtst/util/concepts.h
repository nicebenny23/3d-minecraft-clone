#pragma once
#include <concepts>
namespace stn {



	template<typename T>
	concept CalledWhileIncomplete =! requires { sizeof(T); };
	template<typename From, typename To>
	concept decays_to = std::same_as<std::remove_cvref_t<From>, To>;
	//
	template<typename T, typename... Args>
	concept RecursiveConstructible =
		//skip the call if incomplete
		stn::CalledWhileIncomplete<T>
		// If Complete run the strict check
		|| std::is_constructible_v<T, Args...>;
	template<typename T>
	concept CompleteVirtual = !CalledWhileIncomplete<T> && std::has_virtual_destructor_v<T>;



	template<typename T,typename ...Args>
	concept OneOf= (std::same_as<T,Args>||...);

	template<typename...>
	struct all_unique_impl :std::true_type {

	};

	template<typename Head,typename... Tail>
	struct all_unique_impl<Head,Tail...> :std::bool_constant<
	(!OneOf<Head, Tail...>&& all_unique_impl<Tail...>::value)> {

	};
	template<typename ...Args>
	concept AllUnique = all_unique_impl<Args...>::value;

	template <typename T, template <typename...> class Template>
	struct is_type_instantiation_of : std::false_type {
	};

	template <template <typename...> class Template, typename... Args>
	struct is_type_instantiation_of<Template<Args...>, Template> : std::true_type {
	};
	template <typename T, template <typename...> class Template>
	concept TypeInstantiationOf = is_type_instantiation_of<T, Template>::value;

	template<typename Func, typename ...Args>
	concept nonvoid_invokable = std::invocable<Func, Args&&...> && !std::is_void_v<std::invoke_result_t<Func, Args&&...>>;
}