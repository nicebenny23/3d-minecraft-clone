#pragma once
#include "json_value.h"
namespace json {
	Value parse_json(std::string_view view);
	inline Value parse_for_file(const std::filesystem::path& path) {
		std::string json_text = stn::file_handle(path, stn::FileMode::Read).read_text();
		return parse_json(std::string_view(json_text));
	}
	template<typename T>
	concept Parser = stn::nonvoid_invokable<T, const Value&>;
	
	template<Parser T>
	using ParserResult = std::invoke_result_t<T, const Value&>;
	template<Parser T>
	struct ParseOption {
		stn::Option<ParserResult<T>> operator()(const Value& value) {			
			if (value.is<stn::NoneType>()) {
				return stn::None;
			}
			return parser(value);
		}
		ParseOption(T& parse) :parser(parse) {

		}
		T parser;
	};
	template<Parser T>
	struct ParseArray {
		stn::array<ParserResult<T>> operator()(const Value& value) {
			using R = ParserResult<T>;
			stn::array<R> recipes;
			const json::Array& arr = value.extract<json::Array>().expect("must be an array of recipes to parse as an array");
			for (const stn::box<json::Value>& val : arr) {
				recipes.push(parser(*val));
			}
			return recipes;
		}
		
		ParseArray(T&& parse) :parser(std::move(parse)) {

		}
		ParseArray(T& parse) :parser(parse) {

		}
		T parser;
	};

	std::string to_string(const Value& v);
}
template <>
struct std::formatter<json::Value> : std::formatter<std::string> {
	auto format(const json::Value& v, format_context& ctx) const {
		return std::formatter<std::string>::format(
			json::to_string(v),
			ctx
		);
	}
};