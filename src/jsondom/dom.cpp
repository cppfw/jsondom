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
			this->val.boolean = v.val.boolean;
			break;
		case value_type::string:
		case value_type::number:
			new(&this->val.string)std::string(std::move(v.val.string));
			break;
		case value_type::object:
			new(&this->val.object)decltype(this->val.object)(std::move(v.val.object));
			break;
		case value_type::array:
			new(&this->val.array)decltype(this->val.array)(std::move(v.val.array));
			break;
	}
	v.type = value_type::null;
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
			new(&this->val.string)std::string();
			break;
		case value_type::object:
			new(&this->val.object)decltype(this->val.object)();
			break;
		case value_type::array:
			new(&this->val.array)decltype(this->val.array)();
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
	// TODO:
	ASSERT(p.stack.size() == 1)
	return std::move(p.stack.front());
}

void jsondom::write(papki::file& fi, const jsondom::value& v){
	// TODO:
}
