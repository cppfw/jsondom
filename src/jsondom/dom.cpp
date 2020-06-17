#include "dom.hpp"

#include <utki/string.hpp>

#include "parser.hpp"

using namespace jsondom;

value::value(value&& v) :
		stored_type(v.stored_type)
{
	switch(this->stored_type){
		case value_type::null:
			return;
		case value_type::boolean:
			this->var.boolean = v.var.boolean;
			break;
		case value_type::number:
			new(&this->var.number)string_number(std::move(v.var.number));
			break;
		case value_type::string:
			new(&this->var.string)std::string(std::move(v.var.string));
			break;
		case value_type::object:
			new(&this->var.object)decltype(this->var.object)(std::move(v.var.object));
			break;
		case value_type::array:
			new(&this->var.array)decltype(this->var.array)(std::move(v.var.array));
			break;
	}
	v.stored_type = value_type::null;
}

void value::init(const value& v){
	this->stored_type = v.stored_type;
	switch(this->stored_type){
		case value_type::null:
			break;
		case value_type::boolean:
			this->var.boolean = v.var.boolean;
			break;
		case value_type::number:
			new(&this->var.number)string_number(v.var.number);
			break;
		case value_type::string:
			new(&this->var.string)std::string(v.var.string);
			break;
		case value_type::object:
			new(&this->var.object)decltype(this->var.object)(v.var.object);
			break;
		case value_type::array:
			new(&this->var.array)decltype(this->var.array)(v.var.array);
			break;
	}
}

value& value::operator=(const value& v){
	this->~value();
	this->init(v);
	return *this;
}

value::value(const value& v){
	this->init(v);
}

value::value(value_type type) :
	stored_type(type)
{
	switch(this->stored_type){
		case value_type::null:
		case value_type::boolean:
			break;
		case value_type::number:
			new(&this->var.number)string_number();
			break;
		case value_type::string:
			new(&this->var.string)std::string();
			break;
		case value_type::object:
			new(&this->var.object)decltype(this->var.object)();
			break;
		case value_type::array:
			new(&this->var.array)decltype(this->var.array)();
			break;
	}
}

value::value(std::string&& str) :
		stored_type(value_type::string)
{
	new(&this->var.string)std::string(std::move(str));
}

value::value(string_number&& n) :
		stored_type(value_type::number)
{
	new(&this->var.number)string_number(std::move(n));
}

value::value(bool b) :
		stored_type(value_type::boolean)
{
	this->var.boolean = b;
}

value::~value()noexcept{
	switch(this->stored_type){
		case value_type::null:
		case value_type::boolean:
			break;
		case value_type::number:
			this->var.number.~string_number();
			break;
		case value_type::string:
			this->var.string.~basic_string<char>();
			break;
		case value_type::object:
			this->var.object.~map<std::string, value>();
			break;
		case value_type::array:
			this->var.array.~vector<value>();
			break;
	}
}

namespace{
std::string type_to_name(value_type type){
	switch(type){
		case value_type::null:
			return "null";
		case value_type::boolean:
			return "boolean";
		case value_type::number:
			return "number";
		case value_type::string:
			return "string";
		case value_type::object:
			return "object";
		case value_type::array:
			return "array";
	}
	return nullptr;
}
}

void value::throw_access_error(value_type tried_access)const{
	std::stringstream ss;
	ss << "jsondom: could not access " << type_to_name(tried_access) << "value, stored value is of another type (" << type_to_name(this->type()) << ")";
	throw std::logic_error(ss.str());
}

namespace{
struct dom_parser : public parser{
	value doc{value_type::array};

	std::string key;

	std::vector<value*> stack = {&this->doc};

	void on_object_start()override{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch(back->type()){
			case value_type::array:
				back->array().emplace_back(value_type::object);
				this->stack.push_back(&back->array().back());
				break;
			case value_type::object:
				back->object()[this->key] = value(value_type::object);
				this->stack.push_back(&back->object()[this->key]);
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_object_end()override{
		this->stack.pop_back();
	}

	void on_array_start()override{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch(back->type()){
			case value_type::array:
				back->array().emplace_back(value_type::array);
				this->stack.push_back(&back->array().back());
				break;
			case value_type::object:
				back->object()[this->key] = value(value_type::array);
				this->stack.push_back(&back->object()[this->key]);
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_array_end()override{
		this->stack.pop_back();
	}

	void on_key_parsed(utki::span<const char> str)override{
		this->key = utki::make_string(str);
	}

	void on_string_parsed(utki::span<const char> str)override{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch(back->type()){
			case value_type::array:
				back->array().emplace_back(utki::make_string(str));
				break;
			case value_type::object:
				back->object()[this->key] = value(utki::make_string(str));
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_number_parsed(utki::span<const char> str)override{
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch(back->type()){
			case value_type::array:
				back->array().emplace_back(string_number(utki::make_string(str)));
				break;
			case value_type::object:
				back->object()[this->key] = value(string_number(utki::make_string(str)));
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_boolean_parsed(bool b){
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch(back->type()){
			case value_type::array:
				back->array().emplace_back(b);
				break;
			case value_type::object:
				back->object()[this->key] = value(b);
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}

	void on_null_parsed(){
		ASSERT(!this->stack.empty())
		auto back = this->stack.back();
		switch(back->type()){
			case value_type::array:
				back->array().emplace_back();
				break;
			case value_type::object:
				back->object()[this->key] = value();
				this->key.clear();
				break;
			default:
				ASSERT(false)
				break;
		}
	}
};
}

jsondom::value jsondom::read(const papki::file& fi){
	dom_parser p;
	
	{
		papki::file::guard file_guard(fi);

		std::array<uint8_t, 4096> buf; // 4k

		while(true){
			auto res = fi.read(utki::make_span(buf));
			ASSERT_ALWAYS(res <= buf.size())
			if(res == 0){
				break;
			}
			p.feed(utki::make_span(buf.data(), res));
		}
	}

	ASSERT_INFO(p.stack.size() == 1, "p.stack.size() = " << p.stack.size())
	ASSERT(p.doc.is<value_type::array>())

	if(p.doc.array().empty()){
		return value();
	}

	return std::move(p.doc.array().front());
}

namespace{
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
}

namespace{
void write_internal(papki::file& fi, const jsondom::value& v){
	switch(v.type()){
		case value_type::null:
			fi.write(word_null);
			break;
		case value_type::boolean:
			if(v.boolean()){
				fi.write(word_true);
			}else{
				fi.write(word_false);
			}
			break;
		case value_type::number:
			fi.write(utki::make_span(v.number().get_string()));
			break;
		case value_type::string:
			fi.write(double_quote);
			// TODO: escape needed chars in the string
			fi.write(utki::make_span(v.string()));
			fi.write(double_quote);
			break;
		case value_type::array:
			fi.write(open_square_brace);
			for(auto i = v.array().begin(); i != v.array().end(); ++i){
				if(i != v.array().begin()){
					fi.write(comma);
				}
				write_internal(fi, *i);
			}
			fi.write(close_square_brace);
			break;
		case value_type::object:
			fi.write(open_curly_brace);
			for(auto i = v.object().begin(); i != v.object().end(); ++i){
				if(i != v.object().begin()){
					fi.write(comma);
				}
				fi.write(double_quote);
				// TODO: escape needed chars in the string
				fi.write(utki::make_span(i->first));
				fi.write(double_quote_and_colon);
				write_internal(fi, i->second);
			}
			fi.write(close_curly_brace);
			break;
	}
}
}

void jsondom::write(papki::file& fi, const jsondom::value& v){
	if(!v.is<value_type::object>()){
		throw std::logic_error("tried to write JSON with non-object root element");
	}

	papki::file::guard file_guard(fi, papki::file::mode::create);

	write_internal(fi, v);
}

string_number::string_number(int value) :
		string([](int value) -> std::string{
			// TRACE(<< "string_number::string_number(int): value = " << value << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%d", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

string_number::string_number(unsigned char value) :
		string_number((unsigned short int)value)
{}

string_number::string_number(unsigned short int value) :
		string_number((unsigned int)value)
{}

string_number::string_number(unsigned int value) :
		string([](unsigned int value) -> std::string{
			// TRACE(<< "string_number::string_number(uint): value = " << value <<", base = " << int(conversion_base) << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%u", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

string_number::string_number(signed long int value) :
		string([](long int value) -> std::string{
			// TRACE(<< "string_number::string_number(long int): value = " << value << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%ld", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

string_number::string_number(unsigned long int value) :
		string([](unsigned long int value) -> std::string{
			// TRACE(<< "string_number::string_number(ulong): value = " << value << ", base = " << int(conversion_base) << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%lu", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

string_number::string_number(signed long long int value) :
		string([](long long int value) -> std::string{
			// TRACE(<< "string_number::string_number(long long): value = " << value << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%lld", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

string_number::string_number(unsigned long long int value) :
		string([](unsigned long long int value) -> std::string{
			// TRACE(<< "string_number::string_number(u long long): value = " << value << ", base = " << int(conversion_base) << std::endl)
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%llu", value);

			if(0 <= res && res <= int(sizeof(buf))){
				return std::string(buf, res);
			}
			return std::string();
		}(value))
{}

string_number::string_number(float value) :
		string([](float value) -> std::string{
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%.8G", double(value));

			if(res < 0 || res > int(sizeof(buf))){
				return std::string();
			}else{
				return std::string(buf, res);
			}
		}(value))
{}

string_number::string_number(double value) :
		string([](double value) -> std::string{
			char buf[64];

			int res = snprintf(buf, sizeof(buf), "%.17G", value);

			if(res < 0 || res > int(sizeof(buf))){
				return std::string();
			}else{
				return std::string(buf, res);
			}
		}(value))
{}

string_number::string_number(long double value) :
		string([](long double value) -> std::string{
			char buf[128];

			int res = snprintf(buf, sizeof(buf), "%.31LG", value);

			if(res < 0 || res > int(sizeof(buf))){
				return std::string();
			}else{
				return std::string(buf, res);
			}
		}(value))
{}
