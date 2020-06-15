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
public:
	value_type type;

	union variant{
		bool boolean;
		std::string string;
		std::vector<value> array;
		std::map<std::string, value> object;
		variant(){}
		~variant(){}
	}val;

	template <value_type V> bool is()const noexcept{
		return this->type == V;
	}

	value(){}
	value(value&& v);

	value(value_type type);
};

void write(papki::file& fi, const value& v);

inline void write(papki::file&& fi, const value& v){
	jsondom::write(fi, v);
}

value read(const papki::file& fi);

}
