#include "json_value.h"

namespace json {
	//must be defined after completion
	Value::Value(Value&&) noexcept = default;
	Value& Value::operator=(Value&&) noexcept = default;
	Value::~Value() = default;
	
}
