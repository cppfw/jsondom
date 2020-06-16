#pragma once

#include <string>
#include <vector>
#include <map>

#include <papki/file.hpp>

namespace jsondom{

enum class value_type{
	null,
	boolean,
	number,
	string,
	object,
	array
};

class value{
	void init(const value& v);

	value_type stored_type;

	union variant{
		bool boolean;
		std::string string;
		std::vector<value> array;
		std::map<std::string, value> object;
		variant(){}
		~variant(){}
	}var;

	void throw_access_error(value_type tried_access)const;
public:
	value() :
			stored_type(value_type::null)
	{}
	value(const value& v);
	value& operator=(const value& v);
	value(value&& v);
	value(value_type type);
	~value()noexcept;

	value_type type()const noexcept{
		return this->stored_type;
	}

	template <value_type V> bool is()const noexcept{
		return this->stored_type == V;
	}

	bool& boolean(){
		if(!this->is<value_type::boolean>()){
			this->throw_access_error(value_type::boolean);
		}
		return this->var.boolean;
	}

	std::string& string(){
		if(!this->is<value_type::string>()){
			this->throw_access_error(value_type::string);
		}
		return this->var.string;
	}

	decltype(var.array)& array(){
		if(!this->is<value_type::array>()){
			this->throw_access_error(value_type::array);
		}
		return this->var.array;
	}

	decltype(var.object)& object(){
		if(!this->is<value_type::object>()){
			this->throw_access_error(value_type::object);
		}
		return this->var.object;
	}
};

void write(papki::file& fi, const value& v);

inline void write(papki::file&& fi, const value& v){
	jsondom::write(fi, v);
}

value read(const papki::file& fi);

}
