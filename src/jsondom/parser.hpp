#pragma once

#include <vector>

#include <utki/span.hpp>

namespace jsondom{

/**
 * @brief SAX style JSON parser.
 * One has to subclass this class and override on_*() methods, then call feed() method to
 * feed data to the parser.
 */
class parser{
	unsigned line = 1;

	enum class state{
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

	void parse_idle(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_object(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_array(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_key(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_colon(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_value(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_comma(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_string(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_string_escape_sequence(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_unicode_char(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);
	void parse_boolean_or_null_or_number(utki::span<char>::const_iterator& i, utki::span<char>::const_iterator& e);

	void notify_boolean_or_null_or_number_parsed();

	std::vector<char> buf;

	char32_t unicode_char;
	unsigned unicode_char_digit_num;

	void throw_malformed_json_error(char unexpected_char, const std::string& state_name);
public:
	virtual ~parser()noexcept{}

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
	void feed(const utki::span<uint8_t> data){
		this->feed(utki::make_span(reinterpret_cast<const char*>(data.data()), data.size()));
	}
	
	/**
	 * @brief Parse the string.
	 * @param str - string to parse.
	 */
	void feed(const std::string& str){
		this->feed(utki::make_span(str.c_str(), str.length()));
	}
};

}
