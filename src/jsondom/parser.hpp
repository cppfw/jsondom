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

#include <vector>

#include <utki/span.hpp>

namespace jsondom {

/**
 * @brief SAX style JSON parser.
 * One has to subclass this class and override on_*() methods, then call feed() method to
 * feed data to the parser.
 */
class parser
{
	unsigned line = 1;

	enum class state {
		idle,
		object,
		array,
		key,
		colon,
		value,
		comma,
		string,
		string_escape_sequence,
		unicode_char,
		boolean_or_null_or_number
	};

	std::vector<state> state_stack{state::idle};

	void parse_idle(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_object(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_array(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_key(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_colon(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_value(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_comma(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_string(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_string_escape_sequence(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_unicode_char(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);
	void parse_boolean_or_null_or_number(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e);

	void notify_boolean_or_null_or_number_parsed();

	std::vector<char> buf;

	char32_t unicode_char = U'0';
	unsigned unicode_char_digit_num = 0;

	void throw_malformed_json_error(char unexpected_char, const std::string& state_name);

public:
	parser() = default;

	parser(const parser&) = delete;
	parser& operator=(const parser&) = delete;

	parser(parser&&) = delete;
	parser& operator=(parser&&) = delete;

	virtual ~parser() noexcept = default;

	/**
	 * @brief Invoked on JSON object start.
	 * This method is invoked when JSON object start (i.e. '{' symbol)
	 * has been parsed.
	 */
	virtual void on_object_start() = 0;

	/**
	 * @brief Invoked on JSON object end.
	 * This method is invoked when JSON object end (i.e. '}' symbol)
	 * has been parsed.
	 */
	virtual void on_object_end() = 0;

	/**
	 * @brief Invoked on JSON array start.
	 * This method is invoked when JSON array start (i.e. '[' symbol)
	 * has been parsed.
	 */
	virtual void on_array_start() = 0;

	/**
	 * @brief Invoked on JSON array end.
	 * This method is invoked when JSON array end (i.e. ']' symbol)
	 * has been parsed.
	 */
	virtual void on_array_end() = 0;

	/**
	 * @brief Invoked on key from key-value pair.
	 * This method is invoked when key from key-value pair has been parsed
	 * while parsing contents of the JSON object.
	 */
	virtual void on_key_parsed(utki::span<const char> str) = 0;

	/**
	 * @brief Invoked on string value.
	 * This method is invoked when string value has been parsed.
	 * It can be either when parsing JSON object's value from one of its
	 * key-value pairs or it can be during parsing values from JSON array.
	 */
	virtual void on_string_parsed(utki::span<const char> str) = 0;

	/**
	 * @brief Invoked on number value.
	 * This method is invoked when number value has been parsed.
	 * It can be either when parsing JSON object's value from one of its
	 * key-value pairs or it can be during parsing values from JSON array.
	 */
	virtual void on_number_parsed(utki::span<const char> str) = 0;

	/**
	 * @brief Invoked on boolean value.
	 * This method is invoked when boolean value has been parsed.
	 * It can be either when parsing JSON object's value from one of its
	 * key-value pairs or it can be during parsing values from JSON array.
	 */
	virtual void on_boolean_parsed(bool b) = 0;

	/**
	 * @brief Invoked on null value.
	 * This method is invoked when null value has been parsed.
	 * It can be either when parsing JSON object's value from one of its
	 * key-value pairs or it can be during parsing values from JSON array.
	 */
	virtual void on_null_parsed() = 0;

	/**
	 * @brief feed UTF-8 data to parser.
	 * @param data - data to be fed to parser.
	 */
	void feed(utki::span<const char> data);

	/**
	 * @brief feed UTF-8 data to parser.
	 * @param data - data to be fed to parser.
	 */
	void feed(const utki::span<uint8_t> data)
	{
		this->feed(to_char(data));
	}

	/**
	 * @brief Parse the string.
	 * @param str - string to parse.
	 */
	void feed(const std::string& str)
	{
		this->feed(utki::make_span(str.c_str(), str.length()));
	}
};

} // namespace jsondom
