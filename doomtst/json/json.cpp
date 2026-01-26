#include "json.h"
#pragma once
namespace json {

	inline void skip_whitespace(std::string_view& view) {

		while (!view.empty()) {
			char c = view.front();
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
				view.remove_prefix(1);
			}
			else {
				return;
			}
		}
	}
	inline char take_json_front(std::string_view& view) {
		if (view.empty()) {
			stn::throw_logic_error("reached the end of the json without terminating");
		}
		char front = view.front();
		view.remove_prefix(1);
		return front;
	}

	inline bool take_if_match(std::string_view& view, const char* string) {
		size_t len = std::strlen(string);
		if (view.starts_with(string)) {
			view.remove_prefix(len);
			return true;
		}
		return false;
	}
	//skips whitespace than takes if it matches the syntax
	inline bool take_syntax_if_matches(std::string_view& view, const char* string) {
		skip_whitespace(view);
		return take_if_match(view, string);
	}
	Value parse_value(std::string_view& view);
	Value parse_literal(std::string_view& view) {
		if (take_syntax_if_matches(view, "true")) {
			return Value(true);
		}
		if (take_syntax_if_matches(view, "false")) {
			return Value(false);
		}
		if (take_syntax_if_matches(view, "null")) {
			return Value(stn::None);
		}

		stn::throw_logic_error("expected literal 'true' or 'false', got '{}'", view.empty() ? '?' : view.front());
	}

	Value parse_number(std::string_view& view) {
		skip_whitespace(view);
		size_t len = 0;
		while (len < view.length()) {
			char c = view[len];
			if (std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == '.' || c == 'e' || c == 'E') {
				++len;
			}
			else {
				break;
			}
		}
		if (len == 0) stn::throw_logic_error("expected a number");


		std::string num_str(view.substr(0, len));
		view.remove_prefix(len);

		if (num_str.contains('.') ||
			num_str.contains('e') ||
			num_str.contains('E')) {
			return Value(std::stod(num_str));
		}
		else {
			return Value(std::stoll(num_str));
		}
	}
	std::string parse_string(std::string_view& view) {
		std::string str;
		if (!take_syntax_if_matches(view, "\"")) {
			if (view.empty()) {
				stn::throw_logic_error("a string must start with a \"");
			}
			else {
				stn::throw_logic_error("a string must start with a \" not a {}", view.front());
			}
		}
		while (!take_if_match(view, "\"")) {
			if (view.empty()) {
				stn::throw_logic_error("string was not ended with a \"");
			}
			else {
				char front = take_json_front(view);
				if (front == '\\') {
					if (view.empty()) {
						stn::throw_logic_error("string was not ended with a \"");
					}
					front = take_json_front(view);
				}
				str.push_back(front);
			}
		}
		return str;
	}
	Array parse_array(std::string_view& view) {
		if (!take_syntax_if_matches(view, "[")) {
			if (view.empty()) {
				stn::throw_logic_error("json array may not be empty");
			}
			else {
				stn::throw_logic_error("json array must start with [ not {}", view.front());
			}
		}

		stn::array<stn::box<Value>> array;
		if (take_syntax_if_matches(view, "]")) {
			return Array(std::move(array));
		}
		while (true) {
			Value v = parse_value(view);
			array.emplace(std::move(v));
			if (take_syntax_if_matches(view, "]")) {
				return array;
			}
			if (!take_syntax_if_matches(view, ",")) {
				stn::throw_logic_error("expected ',' or ']' in array, got '{}'", view.empty() ? '?' : view.front());
			}

		}
	}
	Object parse_object(std::string_view& view) {
		if (!take_syntax_if_matches(view, "{")) {
			if (view.empty()) {
				stn::throw_logic_error("JSON object must start with '{{'");
			}
			else {
				stn::throw_logic_error("JSON object must start with '{{' not {{}}", view.front());
			}
		}
		std::map<std::string, stn::box<Value>> obj;
		if (take_syntax_if_matches(view, "}")) {
			return Object(std::move(obj));
		}
		while (true) {
			auto key = parse_string(view);
			if (!take_syntax_if_matches(view, ":")) {
				if (view.empty()) {
					stn::throw_logic_error("JSON object must be seperated with ':'");
				}
				else {
					stn::throw_logic_error("JSON object must be seperated with ':' not {}", view.front());
				}
			}
			obj.emplace(key, parse_value(view));
			if (view.empty()) {
				stn::throw_logic_error("json was not properly closed");
			}
			if (take_syntax_if_matches(view, "}")) {
				return obj;
			}
			if (!take_syntax_if_matches(view, ",")) {
				stn::throw_logic_error("json may not be delineated by {}", view.front());
			}


		}

	}

	Value parse_value(std::string_view& view) {
		skip_whitespace(view);
		if (view.empty()) {
			stn::throw_logic_error("json was not properly closed");
		}
		switch (view.front()) {
		case'{':
		{
			return parse_object(view);
		}
		case '[': {
			return parse_array(view);
		}
		case '"': {
			return parse_string(view);
		}
		default:
		if (view.starts_with("true") || view.starts_with("false")||view.starts_with("null")) {
			return parse_literal(view);
		}
		return parse_number(view);
		}
	}



	Value json::parse_json(std::string_view view) {

		return parse_value(view);

	}

}