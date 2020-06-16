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
		case value_type::string:
		case value_type::number:
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
		case value_type::string:
		case value_type::number:
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
		case value_type::string:
		case value_type::number:
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

value::~value()noexcept{
	switch(this->stored_type){
		case value_type::null:
		case value_type::boolean:
			break;
		case value_type::string:
		case value_type::number:
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
	std::string name;

	std::vector<value> stack = {value(value_type::array)};

	void on_object_start()override{

	}

	void on_object_end()override{

	}

	void on_array_start()override{

	}

	void on_array_end()override{

	}

	void on_name_parsed(utki::span<const char> str)override{
		this->name = utki::make_string(str);
	}

	void on_string_parsed(utki::span<const char> str)override{

	}

	void on_number_parsed(utki::span<const char> str)override{

	}

	void on_boolean_parsed(bool b){

	}

	void on_null_parsed(){

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

	ASSERT(p.stack.size() == 1)
	return std::move(p.stack.front());
}

void jsondom::write(papki::file& fi, const jsondom::value& v){
	// TODO:
}
