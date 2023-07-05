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

#include "parser.hpp"

#include <sstream>

#include <utki/string.hpp>
#include <utki/unicode.hpp>

#include "malformed_json_error.hpp"

using namespace jsondom;

namespace {
bool is_dec_digit(char c)
{
	return '0' <= c && c <= '9';
}
} // namespace

namespace {
bool is_hex_digit(char c)
{
	return is_dec_digit(c) || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}
} // namespace

namespace {
uint32_t hex_digit_to_number(char c)
{
	if (is_dec_digit(c)) {
		return c - '0';
	}
	if ('a' <= c && c <= 'f') {
		return utki::to_int(utki::integer_base::dec) + (c - 'a');
	}
	if ('A' <= c && c <= 'F') {
		return utki::to_int(utki::integer_base::dec) + (c - 'A');
	}
	throw std::logic_error(std::string());
}
} // namespace

void parser::throw_malformed_json_error(char unexpected_char, const std::string& state_name)
{
	std::stringstream ss;
	ss << "unexpected character '" << unexpected_char << "' encountered while in " << state_name
	   << " state, line = " << this->line;
	throw malformed_json_error(ss.str());
}

void parser::feed(utki::span<const char> data)
{
	for (auto i = data.begin(), e = data.end(); i != e; ++i) {
		ASSERT(!this->state_stack.empty())
		switch (this->state_stack.back()) {
			case state::idle:
				this->parse_idle(i, e);
				break;
			case state::object:
				this->parse_object(i, e);
				break;
			case state::array:
				this->parse_array(i, e);
				break;
			case state::key:
				this->parse_key(i, e);
				break;
			case state::colon:
				this->parse_colon(i, e);
				break;
			case state::value:
				this->parse_value(i, e);
				break;
			case state::comma:
				this->parse_comma(i, e);
				break;
			case state::string:
				this->parse_string(i, e);
				break;
			case state::string_escape_sequence:
				this->parse_string_escape_sequence(i, e);
				break;
			case state::unicode_char:
				this->parse_unicode_char(i, e);
				break;
			case state::boolean_or_null_or_number:
				this->parse_boolean_or_null_or_number(i, e);
				break;
		}
		if (i == e) {
			return;
		}
	}
}

void parser::parse_idle(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->buf.empty())
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case '{':
				this->state_stack.push_back(state::object);
				this->on_object_start();
				return;
			default:
				this->throw_malformed_json_error(*i, "idle");
				break;
		}
	}
}

void parser::parse_object(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->buf.empty())
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case '}':
				this->state_stack.pop_back();
				this->on_object_end();
				return;
			case '"':
				this->state_stack.push_back(state::key);
				return;
			default:
				this->throw_malformed_json_error(*i, "object");
				break;
		}
	}
}

void parser::parse_key(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
			case '\\':
				this->state_stack.push_back(state::string_escape_sequence);
				return;
			default:
				this->buf.push_back(*i);
				break;
			case '"':
				this->state_stack.pop_back();
				this->on_key_parsed(utki::make_span(this->buf));
				this->buf.clear();
				this->state_stack.push_back(state::colon);
				return;
		}
	}
}

void parser::parse_colon(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->buf.empty())
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case ':':
				this->state_stack.pop_back();
				this->state_stack.push_back(state::value);
				return;
			default:
				this->throw_malformed_json_error(*i, "colon");
				break;
		}
	}
}

void parser::parse_value(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->buf.empty())
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case '{':
				this->state_stack.pop_back();
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::object);
				this->on_object_start();
				return;
			case '[':
				this->on_array_start();
				this->state_stack.pop_back();
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::array);
				return;
			case '"':
				this->state_stack.pop_back();
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::string);
				return;
			case 't': // first letter of 'false' word
			case 'f': // first letter of 'true' word
			case 'n': // first letter of 'null' word
				ASSERT(this->buf.empty())
				this->buf.push_back(*i);
				this->state_stack.pop_back();
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::boolean_or_null_or_number);
				return;
			default:
				if (('0' <= *i && *i <= '9') || *i == '-') { // looks like a number
					this->buf.push_back(*i);
					this->state_stack.pop_back();
					this->state_stack.push_back(state::comma);
					this->state_stack.push_back(state::boolean_or_null_or_number);
					return;
				} else {
					this->throw_malformed_json_error(*i, "value");
				}
				break;
		}
	}
}

void parser::parse_array(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->buf.empty())
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case '{':
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::object);
				this->on_object_start();
				return;
			case '[':
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::array);
				this->on_array_start();
				return;
			case '"':
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::string);
				return;
			case ']':
				this->state_stack.pop_back();
				this->on_array_end();
				return;
			case 't': // first letter of 'true' word
			case 'f': // first letter of 'false' word
			case 'n': // first letter of 'null' word
				ASSERT(this->buf.empty())
				this->buf.push_back(*i);
				this->state_stack.push_back(state::comma);
				this->state_stack.push_back(state::boolean_or_null_or_number);
				return;
			default:
				if (('0' <= *i && *i <= '9') || *i == '-') { // looks like a number
					this->buf.push_back(*i);
					this->state_stack.push_back(state::comma);
					this->state_stack.push_back(state::boolean_or_null_or_number);
					return;
				} else {
					this->throw_malformed_json_error(*i, "array");
				}
				break;
		}
	}
}

void parser::parse_string(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		switch (*i) {
			case '\n':
				++this->line;
			default:
				this->buf.push_back(*i);
				break;
			case '\\':
				this->state_stack.push_back(state::string_escape_sequence);
				return;
			case '"':
				this->state_stack.pop_back();
				this->on_string_parsed(utki::make_span(this->buf));
				this->buf.clear();
				return;
		}
	}
}

void parser::parse_comma(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->buf.empty())
		switch (*i) {
			case '\n':
				++this->line;
			case ' ':
			case '\r':
			case '\t':
				break;
			case ',':
				this->state_stack.pop_back();
				return;
			case '}':
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				if (this->state_stack.back() != state::object) {
					this->throw_malformed_json_error(*i, "comma");
				}
				this->state_stack.pop_back();
				this->on_object_end();
				return;
			case ']':
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				if (this->state_stack.back() != state::array) {
					this->throw_malformed_json_error(*i, "comma");
				}
				this->state_stack.pop_back();
				this->on_array_end();
				return;
			default:
				this->throw_malformed_json_error(*i, "comma");
				break;
		}
	}
}

void parser::parse_boolean_or_null_or_number(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		switch (*i) {
			case '\n':
				++this->line;
			case '\r':
			case '\t':
			case ' ':
				this->notify_boolean_or_null_or_number_parsed();
				this->state_stack.pop_back();
				return;
			case ',':
				this->notify_boolean_or_null_or_number_parsed();
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				ASSERT(this->state_stack.back() == state::comma)
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				return;
			case ']':
				this->notify_boolean_or_null_or_number_parsed();
				this->on_array_end();
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				ASSERT(this->state_stack.back() == state::comma)
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				if (this->state_stack.back() != state::array) {
					this->throw_malformed_json_error('}', "boolean or null or number");
				}
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				return;
			case '}':
				this->notify_boolean_or_null_or_number_parsed();
				this->on_object_end();
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				ASSERT(this->state_stack.back() == state::comma)
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				if (this->state_stack.back() != state::object) {
					this->throw_malformed_json_error('}', "boolean or null or number");
				}
				this->state_stack.pop_back();
				ASSERT(!this->state_stack.empty())
				return;
			default:
				this->buf.push_back(*i);
				break;
		}
	}
}

namespace {
bool is_valid_number_string(const std::string& s)
{
	enum class state {
		idle,
		sign,
		integer,
		dot,
		fraction,
		exponent,
		exponent_sign,
		exponent_integer
	} cur_state = state::idle;

	for (auto c : s) {
		switch (cur_state) {
			case state::idle:
				if (c == '-') {
					cur_state = state::sign; // sign parsed
					break;
				} else if (is_dec_digit(c)) {
					cur_state = state::integer;
					break;
				}
				return false;
			case state::sign:
				if (is_dec_digit(c)) {
					cur_state = state::integer;
					break;
				}
				return false;
			case state::integer:
				if (is_dec_digit(c)) {
					break;
				} else if (c == '.') {
					cur_state = state::dot; // dot parsed
					break;
				} else if (c == 'e' || c == 'E') {
					cur_state = state::exponent; // exponent parsed
					break;
				}
				return false;
			case state::dot:
				if (is_dec_digit(c)) {
					cur_state = state::fraction;
					break;
				}
				return false;
			case state::fraction:
				if (is_dec_digit(c)) {
					break;
				} else if (c == 'e' || c == 'E') {
					cur_state = state::exponent; // exponent parsed
					break;
				}
				return false;
			case state::exponent:
				if (c == '-' || c == '+') {
					cur_state = state::exponent_sign; // exponent sign parsed
					break;
				} else if (is_dec_digit(c)) {
					cur_state = state::exponent_integer;
					break;
				}
				return false;
			case state::exponent_sign:
				if (is_dec_digit(c)) {
					cur_state = state::exponent_integer;
					break;
				}
				return false;
			case state::exponent_integer:
				if (is_dec_digit(c)) {
					break;
				}
				return false;
		}
	}

	if (cur_state == state::sign || cur_state == state::dot || cur_state == state::exponent
		|| cur_state == state::exponent_sign)
	{
		return false;
	}

	return true;
}
} // namespace

void parser::notify_boolean_or_null_or_number_parsed()
{
	auto s = utki::make_string(this->buf);
	if (s == "true") {
		this->on_boolean_parsed(true);
	} else if (s == "false") {
		this->on_boolean_parsed(false);
	} else if (s == "null") {
		this->on_null_parsed();
	} else if (is_valid_number_string(s)) {
		this->on_number_parsed(utki::make_span(this->buf));
	} else {
		std::stringstream ss;
		ss << "unexpected string (" << s << ") encountered while parsing boolean or null or number at line "
		   << this->line;
		throw malformed_json_error(ss.str());
	}
	this->buf.clear();
}

void parser::parse_string_escape_sequence(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		switch (*i) {
			case 'n':
				this->buf.push_back('\n');
				this->state_stack.pop_back();
				return;
			case 'r':
				this->buf.push_back('\r');
				this->state_stack.pop_back();
				return;
			case '\\':
				this->buf.push_back('\\');
				this->state_stack.pop_back();
				return;
			case '/':
				this->buf.push_back('/');
				this->state_stack.pop_back();
				return;
			case 't':
				this->buf.push_back('\t');
				this->state_stack.pop_back();
				return;
			case 'f':
				this->buf.push_back('\f');
				this->state_stack.pop_back();
				return;
			case 'b':
				this->buf.push_back('\b');
				this->state_stack.pop_back();
				return;
			case '"':
				this->buf.push_back('"');
				this->state_stack.pop_back();
				return;
			case 'u':
				this->unicode_char_digit_num = 0;
				this->unicode_char = 0;
				this->state_stack.pop_back();
				this->state_stack.push_back(state::unicode_char);
				return;
			default:
				this->throw_malformed_json_error(*i, "string escape sequence");
				break;
		}
	}
}

void parser::parse_unicode_char(utki::span<const char>::iterator& i, utki::span<const char>::iterator& e)
{
	for (; i != e; ++i) {
		ASSERT(this->unicode_char_digit_num < 4)

		if (!is_hex_digit(*i)) {
			this->throw_malformed_json_error(*i, "unicode character");
		}

		this->unicode_char |= (hex_digit_to_number(*i) << ((3 - this->unicode_char_digit_num) * 4));
		++this->unicode_char_digit_num;

		if (this->unicode_char_digit_num == 4) {
			auto bytes = utki::to_utf8(this->unicode_char);

			for (auto c : bytes) {
				if (c == 0) {
					break;
				}
				this->buf.push_back(c);
			}

			this->state_stack.pop_back();
			return;
		}
	}
}
