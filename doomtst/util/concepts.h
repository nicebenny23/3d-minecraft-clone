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
}