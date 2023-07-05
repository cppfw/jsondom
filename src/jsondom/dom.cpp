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

#include "dom.hpp"

#include <papki/span_file.hpp>
#include <papki/vector_file.hpp>
#include <utki/string.hpp>

#include "parser.hpp"

using namespace jsondom;

value::value(jsondom::type type) :
	var([type]() {
		switch (type) {
			default:
			case jsondom::type::null:
				return variant_type(nullptr);
			case jsondom::type::boolean:
				return variant_type(false);
			case jsondom::type::number:
				return variant_type(string_number(0));
			case jsondom::type::string:
				return variant_type(std::string());
			case jsondom::type::object:
				return variant_type(object_type());
			case jsondom::type::array:
				return variant_type(array_type());
		}
	}())
{}

namespace {
std::string type_to_name(type type)
{
	switch (type) {
		default:
		case type::null:
			return "null";
		case type::boolean:
			return "boolean";
		case type::number:
			return "number";
		case type::string:
			return "string";
		case type::object:
			return "object";
		case type::array:
			return "array";
	}
}
} // namespace

void value::throw_access_error(type tried_access) const
{
	std::stringstream ss;
	ss << "jsondom: could not access " << type_to_name(tried_access) << "value, stored value is of another type ("
	   << type_to_name(this->get_type()) << ")";
	throw std::logic_error(ss.str());
}

namespace {
struct dom_parser : public parser {
	value doc{type::array};

	std::string key;

	std::vector<value*> stack = {&this->doc};

	void on_object_start() override
	{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch (back->get_type()) {
			case type::array:
				back->array().emplace_back(type::object);
				this->stack.push_back(&back->array().back());
				break;
			case type::object:
				back->object()[this->key] = value(type::object);
				this->stack.push_back(&back->object()[this->key]);
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_object_end() override
	{
		this->stack.pop_back();
	}

	void on_array_start() override
	{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch (back->get_type()) {
			case type::array:
				back->array().emplace_back(type::array);
				this->stack.push_back(&back->array().back());
				break;
			case type::object:
				back->object()[this->key] = value(type::array);
				this->stack.push_back(&back->object()[this->key]);
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_array_end() override
	{
		this->stack.pop_back();
	}

	void on_key_parsed(utki::span<const char> str) override
	{
		this->key = utki::make_string(str);
	}

	void on_string_parsed(utki::span<const char> str) override
	{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch (back->get_type()) {
			case type::array:
				back->array().emplace_back(utki::make_string(str));
				break;
			case type::object:
				back->object()[this->key] = value(utki::make_string(str));
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_number_parsed(utki::span<const char> str) override
	{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch (back->get_type()) {
			case type::array:
				back->array().emplace_back(string_number(utki::make_string(str)));
				break;
			case type::object:
				back->object()[this->key] = value(string_number(utki::make_string(str)));
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_boolean_parsed(bool b) override
	{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch (back->get_type()) {
			case type::array:
				back->array().emplace_back(b);
				break;
			case type::object:
				back->object()[this->key] = value(b);
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_null_parsed() override
	{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch (back->get_type()) {
			case type::array:
				back->array().emplace_back();
				break;
			case type::object:
				back->object()[this->key] = value();
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}
};
} // namespace

jsondom::value jsondom::read(const papki::file& fi)
{
	dom_parser p;

	{
		papki::file::guard file_guard(fi);

		std::array<uint8_t, 4096> buf; // 4k

		while (true) {
			auto res = fi.read(utki::make_span(buf));
			utki::assert(res <= buf.size(), SL);
			if (res == 0) {
				break;
			}
			p.feed(utki::make_span(buf.data(), res));
		}
	}

	ASSERT(p.stack.size() == 1, [&](auto& o) {
		o << "p.stack.size() = " << p.stack.size();
	})
	ASSERT(p.doc.is<type::array>())

	if (p.doc.array().empty()) {
		return {};
	}

	return std::move(p.doc.array().front());
}

jsondom::value jsondom::read(utki::span<const char> data)
{
	const papki::span_file fi(data);

	return read(fi);
}

jsondom::value jsondom::read(utki::span<const uint8_t> data)
{
	const papki::span_file fi(data);

	return read(fi);
}

jsondom::value jsondom::read(const char* str)
{
	if (!str) {
		return {};
	}

	// TODO: optimize
	size_t len = strlen(str);

	return read(utki::make_span(str, len));
}

namespace {
auto open_curly_brace = utki::make_span("{");
auto close_curly_brace = utki::make_span("}");
auto open_square_brace = utki::make_span("[");
auto close_square_brace = utki::make_span("]");
auto double_quote = utki::make_span("\"");
auto double_quote_and_colon = utki::make_span("\":");
auto comma = utki::make_span(",");
auto word_true = utki::make_span("true");
auto word_false = utki::make_span("false");
auto word_null = utki::make_span("null");
} // namespace

namespace {
std::string escape_string(const std::string& str)
{
	std::stringstream ss;

	for (auto c : str) {
		switch (c) {
			case '"':
				ss << '\\' << '"';
				break;
			case '\\':
				ss << '\\' << '\\';
				break;
			case '/':
				ss << '\\' << '/';
				break;
			case '\b':
				ss << '\\' << 'b';
				break;
			case '\f':
				ss << '\\' << 'f';
				break;
			case '\n':
				ss << '\\' << 'n';
				break;
			case '\r':
				ss << '\\' << 'r';
				break;
			case '\t':
				ss << '\\' << 't';
				break;
			default:
				ss << c;
				break;
		}
	}

	return ss.str();
}
} // namespace

namespace {
void write_internal(papki::file& fi, const jsondom::value& v)
{
	switch (v.get_type()) {
		default:
		case type::null:
			fi.write(word_null);
			break;
		case type::boolean:
			if (v.boolean()) {
				fi.write(word_true);
			} else {
				fi.write(word_false);
			}
			break;
		case type::number:
			fi.write(utki::make_span(v.number().get_string()));
			break;
		case type::string:
			{
				fi.write(double_quote);
				fi.write(escape_string(v.string()));
				fi.write(double_quote);
				break;
			}
		case type::array:
			fi.write(open_square_brace);
			for (auto i = v.array().begin(); i != v.array().end(); ++i) {
				if (i != v.array().begin()) {
					fi.write(comma);
				}
				write_internal(fi, *i);
			}
			fi.write(close_square_brace);
			break;
		case type::object:
			fi.write(open_curly_brace);
			for (auto i = v.object().begin(); i != v.object().end(); ++i) {
				if (i != v.object().begin()) {
					fi.write(comma);
				}
				fi.write(double_quote);
				fi.write(escape_string(i->first));
				fi.write(double_quote_and_colon);
				write_internal(fi, i->second);
			}
			fi.write(close_curly_brace);
			break;
	}
}
} // namespace

void jsondom::write(papki::file& fi, const jsondom::value& v)
{
	if (!v.is<type::object>()) {
		throw std::logic_error("tried to write JSON with non-object root element");
	}

	papki::file::guard file_guard(fi, papki::file::mode::create);

	write_internal(fi, v);
}

string_number::string_number(int value) :
	string([](int value) -> std::string {
		// TRACE(<< "string_number::string_number(int): value = " << value << std::endl)
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%d", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(unsigned char value) :
	string_number((unsigned short int)value)
{}

string_number::string_number(unsigned short int value) :
	string_number((unsigned int)value)
{}

string_number::string_number(unsigned int value) :
	string([](unsigned int value) -> std::string {
		// TRACE(<< "string_number::string_number(uint): value = " << value <<", base = " << int(conversion_base) <<
		// std::endl)
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%u", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(signed long int value) :
	string([](long int value) -> std::string {
		// TRACE(<< "string_number::string_number(long int): value = " << value << std::endl)
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%ld", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(unsigned long int value) :
	string([](unsigned long int value) -> std::string {
		// TRACE(<< "string_number::string_number(ulong): value = " << value << ", base = " << int(conversion_base) <<
		// std::endl)
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%lu", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(signed long long int value) :
	string([](long long int value) -> std::string {
		// TRACE(<< "string_number::string_number(long long): value = " << value << std::endl)
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%lld", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(unsigned long long int value) :
	string([](unsigned long long int value) -> std::string {
		// TRACE(<< "string_number::string_number(u long long): value = " << value << ", base = " <<
		// int(conversion_base) << std::endl)
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%llu", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(float value) :
	string([](float value) -> std::string {
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%.8G", double(value));

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

string_number::string_number(double value) :
	string([](double value) -> std::string {
		std::array<char, 64> buf;

		int res = snprintf(buf.data(), buf.size(), "%.17G", value);

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

string_number::string_number(long double value) :
	string([](long double value) -> std::string {
		std::array<char, 128> buf;

		int res = snprintf(buf.data(), buf.size(), "%.31LG", value);

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

std::string value::to_string() const
{
	papki::vector_file file;
	jsondom::write(file, *this);
	return utki::make_string(file.reset_data());
}
