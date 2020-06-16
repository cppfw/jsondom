#include "dom.hpp"

#include <utki/string.hpp>

#include "parser.hpp"

using namespace jsondom;

value::value(value&& v) :
		type(v.type)
{
	switch(this->type){
		case value_type::null:
			return;
		case value_type::boolean:
			this->boolean = v.boolean;
			break;
		case value_type::string:
		case value_type::number:
			new(&this->string)std::string(std::move(v.string));
			break;
		case value_type::object:
			new(&this->object)decltype(this->object)(std::move(v.object));
			break;
		case value_type::array:
			new(&this->array)decltype(this->array)(std::move(v.array));
			break;
	}
	v.type = value_type::null;
}

void value::init(const value& v){
	this->type = v.type;
	switch(this->type){
		case value_type::null:
			break;
		case value_type::boolean:
			this->boolean = v.boolean;
			break;
		case value_type::string:
		case value_type::number:
			new(&this->string)std::string(v.string);
			break;
		case value_type::object:
			new(&this->object)decltype(this->object)(v.object);
			break;
		case value_type::array:
			new(&this->array)decltype(this->array)(v.array);
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
	type(type)
{
	switch(this->type){
		case value_type::null:
		case value_type::boolean:
			break;
		case value_type::string:
		case value_type::number:
			new(&this->string)std::string();
			break;
		case value_type::object:
			new(&this->object)decltype(this->object)();
			break;
		case value_type::array:
			new(&this->array)decltype(this->array)();
			break;
	}
}

value::~value()noexcept{
	switch(this->type){
		case value_type::null:
		case value_type::boolean:
			break;
		case value_type::string:
		case value_type::number:
			this->string.~basic_string<char>();
			break;
		case value_type::object:
			this->object.~map<std::string, value>();
			break;
		case value_type::array:
			this->array.~vector<value>();
			break;
	}
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
