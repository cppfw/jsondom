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

class string_number{
	std::string string;
	
public:
	string_number(){}

	explicit string_number(std::string&& string) :
			string(std::move(string))
	{}

	explicit string_number(unsigned char value);
    explicit string_number(unsigned short int value);

    explicit string_number(signed int value);
    explicit string_number(unsigned int value);

    explicit string_number(signed long int value);
    explicit string_number(unsigned long int value);

    explicit string_number(signed long long int value);
    explicit string_number(unsigned long long int value);

    explicit string_number(float value);
    explicit string_number(double value);
    explicit string_number(long double value);

	const std::string& get_string()const noexcept{
		return this->string;
	}

	int32_t to_int32()const{
        return int32_t(std::stoi(this->string, nullptr, 0));
    }
    uint32_t to_uint32()const{
        return uint32_t(std::stoul(this->string, nullptr, 0));
    }

    int64_t to_int64()const{
        return int64_t(std::stoll(this->string, nullptr, 0));
    }
    uint64_t to_uint64()const{
        return uint64_t(std::stoull(this->string, nullptr, 0));
    }

    float to_float()const{
        return std::stof(this->string);
    }
    double to_double()const{
        return std::stod(this->string);
    }
    long double to_long_double()const{
        return std::stold(this->string);
    }
};

class value{
	void init(const value& v);

	value_type stored_type;

	union variant{
		bool boolean;
		std::string string;
		std::vector<value> array;
		std::map<std::string, value> object;
		string_number number;
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
	~value()noexcept;

	value(value_type type);
	value(std::string&& str);
	value(string_number&& n);
	value(bool b);

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

	bool boolean()const{
		if(!this->is<value_type::boolean>()){
			this->throw_access_error(value_type::boolean);
		}
		return this->var.boolean;
	}

	string_number& number(){
		if(!this->is<value_type::number>()){
			this->throw_access_error(value_type::number);
		}
		return this->var.number;
	}

	const string_number& number()const{
		if(!this->is<value_type::number>()){
			this->throw_access_error(value_type::number);
		}
		return this->var.number;
	}

	std::string& string(){
		if(!this->is<value_type::string>()){
			this->throw_access_error(value_type::string);
		}
		return this->var.string;
	}

	const std::string& string()const{
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

	const decltype(var.array)& array()const{
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

	const decltype(var.object)& object()const{
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

value read(const char* str);

inline value read(const std::string& str){
	return read(str.c_str());
}

}
