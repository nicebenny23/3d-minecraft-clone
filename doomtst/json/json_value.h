
#include <map>
#include <string>
#include "../util/dynamicarray.h"
#include "../util/fileloader.h"
#include <variant>
#include "../util/unique.h"
#pragma once
namespace json {
	
	struct Value;
	struct Array {
		Array(stn::array<stn::box<Value>>&& value) :array(std::move(value)) {

		}
		
		using iterator = stn::array<stn::box<Value>>::const_iterator;

		iterator begin() const {
			return array.begin();
		}
		iterator end() const {
			return array.end();
		}
		size_t length() const{
			return array.length();
		}
		void push(const Value& val) {

		}
		stn::array<stn::box<Value>> array;
	};
	struct Object {
		Object(std::map<std::string, stn::box<Value>>&& object_map) :map(std::move(object_map)) {

		}

		using iterator = std::map<std::string, stn::box<Value>>::const_iterator;

		iterator begin() const{
			return map.begin();
		}
		iterator end() const {
			return map.end();
		}

		stn::Option<const Value&> get(const std::string& key) const{
			if (map.contains(key)) {
				return *map.at(key);
			}
			return stn::None;
		}
		
		std::map<std::string, stn::box<Value>> map;
	};
	template<typename T>
	concept ValidJsonType =stn::OneOf<T, bool, double, std::int64_t, std::string, Array, Object, stn::NoneType>;

	struct Value {

		std::variant < bool, double, std::int64_t, std::string, Array, Object, stn::NoneType> value;
		template<ValidJsonType T>
		bool is() const {
			return std::holds_alternative<T>(value);
		}
		template<ValidJsonType T>
		stn::Option<T&> extract() {
			if (is<T>(value)) {
				return std::get<T>(value);
			}
			return stn::None;
		}
		
		template<ValidJsonType T>
		stn::Option<const T&> extract() const {
			if (is<T>()) {
				return std::get<T>(value);
			}
			return stn::None;
		}
		stn::Option<double> as_number() const {
			if (auto* d = std::get_if<double>(&value)) {
				return *d;
			}
			if (auto* i = std::get_if<std::int64_t>(&value)) {
				return static_cast<double>(*i);
			}
			return stn::None;

		}
		stn::Option<std::uint64_t> as_unsigned() const {
		
			if (const std::int64_t* i = std::get_if<std::int64_t>(&value)) {
				if (0<=*i) {
					return static_cast<double>(*i);
				}
			}
			return stn::None;

		}
		stn::Option<const Value&> get_subobject(const std::string& key) const {
		return  extract<Object>().
				and_then([key](const Object& object) {return object.get(key);});
		}
		template<ValidJsonType T>
		stn::Option<const T&> get_subobject_as(const std::string& key) const {
			return  get_subobject(key).
				and_then([key](const Value& object)->stn::Option<const T&> {return object.extract<T>(); });
		}
		

		template<ValidJsonType T> requires (!stn::decays_to<T, Value>)
			Value(T&& json_value) :value(std::move(json_value)) {

		}
		template<ValidJsonType T> requires (!stn::decays_to<T, Value>)
			Value(const T& json_value) : value(json_value) {

		}
		Value(Value&&) noexcept;
		Value& operator=(Value&&) noexcept;
		~Value();

		Value(const Value&) = delete;
		Value& operator=(const Value&) = delete;

	};
}