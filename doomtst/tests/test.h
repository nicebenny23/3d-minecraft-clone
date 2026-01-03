#include <string>
#include <functional>
#include "../util/dynamicarray.h"
#include "../util/Option.h"
#pragma once
namespace test {
	using test_pass = stn::NoneType;
	using test_result = stn::Option<std::string>;

	inline test_pass pass() {
		return stn::None;
	}
	template<typename... Args>
	inline test_result fail(const std::string& fmt, Args&&... args) {
		return test_result(std::format(fmt, std::forward<Args>(args)...));
	}
	struct test_case {
		std::string name;
		std::function<stn::Option<std::string>()> test_function;
		template<typename F>
			requires std::invocable<F>&& std::convertible_to<std::invoke_result_t<F>, stn::Option<std::string>>
		test_case(std::string n, F&& f)
			: name(std::move(n)), test_function(std::forward<F>(f)) {
		}
		
	};
	struct Tests {
		static Tests& instance() {
			static Tests test_instance;
			return test_instance;
		}
		void register_test(test_case test) {
			cases.push(test);
		}
		stn::array<test_case> cases;
		void run() {
			for (test_case test:cases ) {
				stn::Option<std::string> result= test.test_function();
				if (result) {
					stn::throw_logic_error("test case {} failed with mesage {}",test.name,result.unwrap());
				}
			}
		}
	};
	struct register_test{
		template<typename F>
			requires std::invocable<F>&& std::convertible_to<std::invoke_result_t<F>, stn::Option<std::string>>
		register_test(std::string name, F&& f) {
			Tests::instance().register_test(test_case(name, std::forward<F>(f)));
		}
	};
}