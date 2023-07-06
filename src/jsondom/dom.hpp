/*
MIT License

Copyright (c) 2020-2021 Ivan Gagis

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/* ================ LICENSE END ================ */

#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

#include <papki/file.hpp>
#include <utki/config.hpp>
#include <utki/types.hpp>

namespace jsondom {

/**
 * @brief Type of the JSON value.
 * JSON specifies only 6 value types, this enumeration
 * lists those 6 types.
 */
enum class type {
	null,
	boolean,
	number,
	string,
	object,
	array,

	enum_size
};

/**
 * @brief JSON number value encapsulation.
 * This class encapsulates a number value as it is stored in JSON document,
 * i.e. in text form. The number can be converted to different integer or floating point
 * number formats.
 */
// TODO: whay does lint on macos complain?
// NOLINTNEXTLINE(bugprone-exception-escape)
class string_number
{
	std::string string;

public:
	// TODO: whay does lint on macos complain?
	// NOLINTNEXTLINE(bugprone-exception-escape)
	string_number() = default;

	explicit string_number(std::string string) :
		string(std::move(string))
	{}

	explicit string_number(unsigned char value);
	explicit string_number(unsigned short int value);

	explicit string_number(signed int value);
	explicit string_number(unsigned int value);

	explicit string_number(signed long int value);
	explicit string_number(unsigned long int value);

	explicit string_number(signed long long int value);
	explicit string_number(unsigned long long int value);

	explicit string_number(float value);
	explicit string_number(double value);
	explicit string_number(long double value);

	/**
	 * @brief Get the number as a string.
	 * This method returns the underlying string which holds the number.
	 */
	const std::string& get_string() const noexcept
	{
		return this->string;
	}

	int32_t to_int32() const
	{
		return int32_t(std::stoi(this->string, nullptr, 0));
	}

	uint32_t to_uint32() const
	{
		return uint32_t(std::stoul(this->string, nullptr, 0));
	}

	int64_t to_int64() const
	{
		return int64_t(std::stoll(this->string, nullptr, 0));
	}

	uint64_t to_uint64() const
	{
		return uint64_t(std::stoull(this->string, nullptr, 0));
	}

	float to_float() const
	{
		return std::stof(this->string);
	}

	double to_double() const
	{
		return std::stod(this->string);
	}

	long double to_long_double() const
	{
		return std::stold(this->string);
	}
};

/**
 * @brief JSON value.
 * This class encapsulates the JSON value along with its type.
 */
class value
{
	using array_type = std::vector<value>;
	using object_type = std::map<std::string, value>;

	using variant_type = std::variant<std::nullptr_t, bool, string_number, std::string, object_type, array_type>;

	// check that variant types order corresponds to type enum order
#if CFG_COMPILER != CFG_COMPILER_MSVC
	static_assert(
		std::is_same_v<
			utki::remove_const_reference_t<decltype(std::get<size_t(jsondom::type::null)>(std::declval<variant_type>())
			)>,
			std::nullptr_t>,
		"type of null variant alternative is not std::nullptr_t"
	);
	static_assert(
		std::is_same_v<
			utki::remove_const_reference_t<
				decltype(std::get<size_t(jsondom::type::boolean)>(std::declval<variant_type>()))>,
			bool>,
		"type of boolean variant alternative is not bool"
	);
	static_assert(
		std::is_same_v<
			utki::remove_const_reference_t<decltype(std::get<size_t(jsondom::type::number)>(std::declval<variant_type>()
			))>,
			string_number>,
		"type of number variant alternative is not string_number"
	);
	static_assert(
		std::is_same_v<
			utki::remove_const_reference_t<decltype(std::get<size_t(jsondom::type::string)>(std::declval<variant_type>()
			))>,
			std::string>,
		"type of string variant alternative is not std::string"
	);
	static_assert(
		std::is_same_v<
			utki::remove_const_reference_t<decltype(std::get<size_t(jsondom::type::object)>(std::declval<variant_type>()
			))>,
			object_type>,
		"type of object variant alternative is not object_type"
	);
	static_assert(
		std::is_same_v<
			utki::remove_const_reference_t<decltype(std::get<size_t(jsondom::type::array)>(std::declval<variant_type>())
			)>,
			array_type>,
		"type of array variant alternative is not array_type"
	);
#endif

	variant_type var;

	void throw_access_error(type tried_access) const;

public:
	value() = default;

	value(const value&) = default;
	value& operator=(const value&) = default;

	value(value&& v) = default;
	value& operator=(value&&) = default;

	~value() = default;

	value(type type);

	value(std::string str) :
		var(str)
	{}

	value(string_number n) :
		var(n)
	{}

	value(bool b) :
		var(b)
	{}

	/**
	 * @brief Get value type.
	 * @return value type.
	 */
	jsondom::type get_type() const noexcept
	{
		ASSERT(this->var.index() < size_t(jsondom::type::enum_size))
		return jsondom::type(this->var.index());
	}

	/**
	 * @brief Check that the value is of the given type.
	 * @return true if the value is of the given type.
	 * @return false otherwise.
	 */
	template <jsondom::type json_type>
	bool is() const noexcept
	{
		return this->var.index() == size_t(json_type);
	}

	/**
	 * @brief Check if the value is of the null type.
	 * @return true if the value is of the null type.
	 * @return false otherwise.
	 */
	bool is_null() const noexcept
	{
		return this->is<type::null>();
	}

	/**
	 * @brief Check if the value is of the boolean type.
	 * @return true if the value is of the boolean type.
	 * @return false otherwise.
	 */
	bool is_boolean() const noexcept
	{
		return this->is<type::boolean>();
	}

	/**
	 * @brief Check if the value is of the number type.
	 * @return true if the value is of the number type.
	 * @return false otherwise.
	 */
	bool is_number() const noexcept
	{
		return this->is<type::number>();
	}

	/**
	 * @brief Check if the value is of the string type.
	 * @return true if the value is of the string type.
	 * @return false otherwise.
	 */
	bool is_string() const noexcept
	{
		return this->is<type::string>();
	}

	/**
	 * @brief Check if the value is of the array type.
	 * @return true if the value is of the array type.
	 * @return false otherwise.
	 */
	bool is_array() const noexcept
	{
		return this->is<type::array>();
	}

	/**
	 * @brief Check if the value is of the object type.
	 * @return true if the value is of the object type.
	 * @return false otherwise.
	 */
	bool is_object() const noexcept
	{
		return this->is<type::object>();
	}

	/**
	 * @brief Get boolean value.
	 * @return reference to the underlying boolean value.
	 * @throw std::logic_error in case the stored value is not a boolean.
	 */
	bool& boolean()
	{
		if (!this->is<type::boolean>()) {
			this->throw_access_error(type::boolean);
		}
		return std::get<bool>(this->var);
	}

	/**
	 * @brief Get constant boolean value.
	 * @return the copy of underlying boolean value.
	 * @throw std::logic_error in case the stored value is not a boolean.
	 */
	bool boolean() const
	{
		if (!this->is<type::boolean>()) {
			this->throw_access_error(type::boolean);
		}
		return std::get<bool>(this->var);
	}

	/**
	 * @brief Get number value.
	 * @return reference to the underlying number value.
	 * @throw std::logic_error in case the stored value is not a number.
	 */
	string_number& number()
	{
		if (!this->is<type::number>()) {
			this->throw_access_error(type::number);
		}
		return std::get<string_number>(this->var);
	}

	/**
	 * @brief Get constant number value.
	 * @return constant reference to the underlying number value.
	 * @throw std::logic_error in case the stored value is not a number.
	 */
	const string_number& number() const
	{
		if (!this->is<type::number>()) {
			this->throw_access_error(type::number);
		}
		return std::get<string_number>(this->var);
	}

	/**
	 * @brief Get string value.
	 * @return reference to the underlying string value.
	 * @throw std::logic_error in case the stored value is not a string.
	 */
	std::string& string()
	{
		if (!this->is<type::string>()) {
			this->throw_access_error(type::string);
		}
		return std::get<std::string>(this->var);
	}

	/**
	 * @brief Get constant string value.
	 * @return constant reference to the underlying string value.
	 * @throw std::logic_error in case the stored value is not a string.
	 */
	const std::string& string() const
	{
		if (!this->is<type::string>()) {
			this->throw_access_error(type::string);
		}
		return std::get<std::string>(this->var);
	}

	/**
	 * @brief Get array value.
	 * @return reference to the underlying array value.
	 * @throw std::logic_error in case the stored value is not an array.
	 */
	array_type& array()
	{
		if (!this->is<type::array>()) {
			this->throw_access_error(type::array);
		}
		return std::get<array_type>(this->var);
	}

	/**
	 * @brief Get constant array value.
	 * @return constant reference to the underlying array value.
	 * @throw std::logic_error in case the stored value is not an array.
	 */
	const array_type& array() const
	{
		if (!this->is<type::array>()) {
			this->throw_access_error(type::array);
		}
		return std::get<array_type>(this->var);
	}

	/**
	 * @brief Get object value.
	 * @return reference to the underlying object value.
	 * @throw std::logic_error in case the stored value is not an object.
	 */
	object_type& object()
	{
		if (!this->is<type::object>()) {
			this->throw_access_error(type::object);
		}
		return std::get<object_type>(this->var);
	}

	/**
	 * @brief Get constant object value.
	 * @return constant reference to the underlying object value.
	 * @throw std::logic_error in case the stored value is not an object.
	 */
	const object_type& object() const
	{
		if (!this->is<type::object>()) {
			this->throw_access_error(type::object);
		}
		return std::get<object_type>(this->var);
	}

	std::string to_string() const;
};

/**
 * @brief Write the JSON document to a file.
 * @param fi - file to write the JSON document to.
 * @param v - root value of the JSON document to write.
 */
void write(papki::file& fi, const value& v);

/**
 * @brief Read JSON document from file.
 * @param fi - file to read the JSON document from.
 * @return the read JSON document.
 */
value read(const papki::file& fi);

/**
 * @brief Read JSON document from memory.
 * @param data - memory span to read the JSON document from.
 * @return the read JSON document.
 */
value read(utki::span<const char> data);

/**
 * @brief Read JSON document from memory.
 * @param data - memory span to read the JSON document from.
 * @return the read JSON document.
 */
value read(utki::span<const uint8_t> data);

/**
 * @brief Read JSON document from string.
 * @param str - string to read the JSON document from.
 * @return the read JSON document.
 */
value read(const char* str);

/**
 * @brief Read JSON document from string.
 * @param str - string to read the JSON document from.
 * @return the read JSON document.
 */
inline value read(const std::string& str)
{
	return read(str.c_str());
}

} // namespace jsondom
