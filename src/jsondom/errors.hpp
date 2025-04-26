/*
MIT License

Copyright (c) 2020-2024 Ivan Gagis

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

#pragma once

#include <stdexcept>
#include <string>
#include <utility>

namespace jsondom {

/**
 * @brief Basic json error.
 */
class error : public std::logic_error
{
public:
	/**
	 * @brief Constructor.
	 * @param message - human readable error message.
	 */
	error(std::string message) :
		std::logic_error(message)
	{}
};

/**
 * @brief Malformed JSON error.
 * This exception is thrown during JSON parsing in case
 * unexpected characters are encountered.
 */
class malformed_json_error : public error
{
public:
	/**
	 * @brief Constructor.
	 * @param message - human readable error message.
	 */
	malformed_json_error(std::string message) :
		error(std::move(message))
	{}
};

/**
 * @brief Unexpected JSON value type error.
 * This error is thrown when a JSON value is accessed as if it was holding a type which it does not hold.
 * For example, if a value holds boolean, but is accessed as if it was an array.
 */
class unexpected_value_type : public error
{
public:
	/**
	 * @brief Constructor.
	 * @param message - human readable error message.
	 */
	unexpected_value_type(std::string message) :
		error(std::move(message))
	{}
};

} // namespace jsondom
