#pragma once
#include "json_value.h"
namespace json {
	Value parse_json(std::string_view view);
	inline Value parse_for_file(const std::filesystem::path& path) {
		std::string json_text = stn::file_handle(path, stn::FileMode::Read).read_text();
		return parse_json(std::string_view(json_text));
	}
	

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