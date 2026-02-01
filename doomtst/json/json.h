#pragma once
#include "json_value.h"
namespace json {
	Value parse_json(std::string_view view);
inline Value parse_for_file(const std::filesystem::path& path) {
	std::string json_text= stn::file_handle(path,stn::FileMode::Read).read_text();
	return parse_json(std::string_view(json_text));
}
	template<typename T>
	concept Parser = stn::HasFunctionTraits<T>&&std::same_as<stn::arg_at_t<T, 0>, const Value&>;
	template<Parser T>
	using ParseResult = stn::result_type_t<T>;
	template<Parser T>
	using NonJsonArguments =
		TypeList::type_list_to_tuple_t<
		TypeList::remove_first_t<
		TypeList::type_list_from_tuple_t<stn::arguments_t<T>>
		>
		>;

	template<Parser T, typename... Rest>
		requires std::same_as<std::tuple<std::remove_cvref_t<Rest>...>, NonJsonArguments<T>>
	stn::array<ParseResult<T>> json_array(const Value& value,Rest&&... extra_args) {
		const Array& as_array = value.extract<Array>().expect("must be parsing an array");
		stn::array<ParseResult<T>> elements;
		for (const stn::box<Value>& array_element:as_array) {
			elements.push(T(array_element, std::forward<Rest>(extra_args)...));
		}
		return elements;
	}
	template<Parser T, typename... Rest>
		requires std::same_as<std::tuple<Rest...>, NonJsonArguments<T>>
	ParseResult<T> parse_json_from_path(T& parser,const std::filesystem::path& path, Rest&&... extra_args) {
		Value val = parse_for_file(path);
		return parser(val, std::forward<Rest>(extra_args)...);
	}
}