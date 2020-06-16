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
public:
	value_type type;

	union{
		bool boolean;
		std::string string;
		std::vector<value> array;
		std::map<std::string, value> object;
	};

	template <value_type V> bool is()const noexcept{
		return this->type == V;
	}

	value(){}
	value(const value& v);
	value& operator=(const value& v);
	value(value&& v);

	value(value_type type);

	~value()noexcept;
};

void write(papki::file& fi, const value& v);

inline void write(papki::file&& fi, const value& v){
	jsondom::write(fi, v);
}

value read(const papki::file& fi);

}
