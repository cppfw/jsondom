#pragma once

#include <string>
#include <vector>
#include <map>

#include <papki/file.hpp>

namespace jsondom{

/**
 * @brief Type of the JSON value.
 * JSON specifies only 6 value types, this enumeration
 * lists those 6 types.
 */
enum class value_type{
	null,
	boolean,
	number,
	string,
	object,
	array
};

/**
 * @brief JSON number value encapsulation.
 * This class encapsulates a number value as it is stored in JSON document,
 * i.e. in text form. The number can be converted to different integer or floating point
 * number formats.
 */
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

	/**
	 * @brief Get the number as a string.
	 * This method returns the underlying string which holds the number.
	 */
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

/**
 * @brief JSON value.
 * This class encapsulates the JSON value along with its type.
 */
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

	/**
	 * @brief Get value type.
	 * @return value type.
	 */
	value_type type()const noexcept{
		return this->stored_type;
	}

	/**
	 * @brief Check that the value is of the given type.
	 * @return true if the value is of the given type.
	 * @return false otherwise.
	 */
	template <value_type V> bool is()const noexcept{
		return this->stored_type == V;
	}

	/**
	 * @brief Check if the value is of the null type.
	 * @return true if the value is of the null type.
	 * @return false otherwise.
	 */
	bool is_null()const noexcept{
		return this->is<value_type::null>();
	}

	/**
	 * @brief Check if the value is of the boolean type.
	 * @return true if the value is of the boolean type.
	 * @return false otherwise.
	 */
	bool is_boolean()const noexcept{
		return this->is<value_type::boolean>();
	}

	/**
	 * @brief Check if the value is of the number type.
	 * @return true if the value is of the number type.
	 * @return false otherwise.
	 */
	bool is_number()const noexcept{
		return this->is<value_type::number>();
	}

	/**
	 * @brief Check if the value is of the string type.
	 * @return true if the value is of the string type.
	 * @return false otherwise.
	 */
	bool is_string()const noexcept{
		return this->is<value_type::string>();
	}

	/**
	 * @brief Check if the value is of the array type.
	 * @return true if the value is of the array type.
	 * @return false otherwise.
	 */
	bool is_array()const noexcept{
		return this->is<value_type::array>();
	}

	/**
	 * @brief Check if the value is of the object type.
	 * @return true if the value is of the object type.
	 * @return false otherwise.
	 */
	bool is_object()const noexcept{
		return this->is<value_type::object>();
	}

	/**
	 * @brief Get boolean value.
	 * @return reference to the underlying boolean value.
	 * @throw std::logic_error in case the stored value is not a boolean.
	 */
	bool& boolean(){
		if(!this->is<value_type::boolean>()){
			this->throw_access_error(value_type::boolean);
		}
		return this->var.boolean;
	}

	/**
	 * @brief Get constant boolean value.
	 * @return the copy of underlying boolean value.
	 * @throw std::logic_error in case the stored value is not a boolean.
	 */
	bool boolean()const{
		if(!this->is<value_type::boolean>()){
			this->throw_access_error(value_type::boolean);
		}
		return this->var.boolean;
	}

	/**
	 * @brief Get number value.
	 * @return reference to the underlying number value.
	 * @throw std::logic_error in case the stored value is not a number.
	 */
	string_number& number(){
		if(!this->is<value_type::number>()){
			this->throw_access_error(value_type::number);
		}
		return this->var.number;
	}

	/**
	 * @brief Get constant number value.
	 * @return constant reference to the underlying number value.
	 * @throw std::logic_error in case the stored value is not a number.
	 */
	const string_number& number()const{
		if(!this->is<value_type::number>()){
			this->throw_access_error(value_type::number);
		}
		return this->var.number;
	}

	/**
	 * @brief Get string value.
	 * @return reference to the underlying string value.
	 * @throw std::logic_error in case the stored value is not a string.
	 */
	std::string& string(){
		if(!this->is<value_type::string>()){
			this->throw_access_error(value_type::string);
		}
		return this->var.string;
	}

	/**
	 * @brief Get constant string value.
	 * @return constant reference to the underlying string value.
	 * @throw std::logic_error in case the stored value is not a string.
	 */
	const std::string& string()const{
		if(!this->is<value_type::string>()){
			this->throw_access_error(value_type::string);
		}
		return this->var.string;
	}

	/**
	 * @brief Get array value.
	 * @return reference to the underlying array value.
	 * @throw std::logic_error in case the stored value is not an array.
	 */
	decltype(var.array)& array(){
		if(!this->is<value_type::array>()){
			this->throw_access_error(value_type::array);
		}
		return this->var.array;
	}

	/**
	 * @brief Get constant array value.
	 * @return constant reference to the underlying array value.
	 * @throw std::logic_error in case the stored value is not an array.
	 */
	const decltype(var.array)& array()const{
		if(!this->is<value_type::array>()){
			this->throw_access_error(value_type::array);
		}
		return this->var.array;
	}

	/**
	 * @brief Get object value.
	 * @return reference to the underlying object value.
	 * @throw std::logic_error in case the stored value is not an object.
	 */
	decltype(var.object)& object(){
		if(!this->is<value_type::object>()){
			this->throw_access_error(value_type::object);
		}
		return this->var.object;
	}

	/**
	 * @brief Get constant object value.
	 * @return constant reference to the underlying object value.
	 * @throw std::logic_error in case the stored value is not an object.
	 */
	const decltype(var.object)& object()const{
		if(!this->is<value_type::object>()){
			this->throw_access_error(value_type::object);
		}
		return this->var.object;
	}
};

/**
 * @brief Write the JSON document to a file.
 * @param fi - file to write the JSON document to.
 * @param v - root value of the JSON document to write.
 */
void write(papki::file& fi, const value& v);

/**
 * @brief Write the JSON document to a file.
 * @param fi - file to write the JSON document to.
 * @param v - root value of the JSON document to write.
 */
inline void write(papki::file&& fi, const value& v){
	jsondom::write(fi, v);
}

/**
 * @brief Read JSON document from file.
 * @param fi - file to read the JSON document from.
 * @return the read JSON document.
 */
value read(const papki::file& fi);

/**
 * @brief Read JSON document from memory.
 * @param data - memory span to read the JSON document from.
 * @return the read JSON document.
 */
value read(utki::span<const char> data);

/**
 * @brief Read JSON document from memory.
 * @param data - memory span to read the JSON document from.
 * @return the read JSON document.
 */
value read(utki::span<const uint8_t> data);

/**
 * @brief Read JSON document from string.
 * @param str - string to read the JSON document from.
 * @return the read JSON document.
 */
value read(const char* str);

/**
 * @brief Read JSON document from string.
 * @param str - string to read the JSON document from.
 * @return the read JSON document.
 */
inline value read(const std::string& str){
	return read(str.c_str());
}

}
