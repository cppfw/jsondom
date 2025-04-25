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

#include <cstdint>
#include <string>

namespace jsondom {

/**
 * @brief JSON number value encapsulation.
 * This class encapsulates a number value as it is stored in JSON document,
 * i.e. in text form. The number can be converted to different integer or floating point
 * number formats.
 */
// TODO: why does lint on macos complain?
// NOLINTNEXTLINE(bugprone-exception-escape)
class string_number
{
	std::string string;

public:
	// TODO: why does lint on macos complain?
	// NOLINTNEXTLINE(bugprone-exception-escape)
	string_number() = default;

	explicit string_number(std::string string) noexcept :
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
	const std::string& get_string() const noexcept
	{
		return this->string;
	}

	int32_t to_int32() const
	{
		return int32_t(std::stoi(this->string, nullptr, 0));
	}

	uint32_t to_uint32() const
	{
		return uint32_t(std::stoul(this->string, nullptr, 0));
	}

	int64_t to_int64() const
	{
		return int64_t(std::stoll(this->string, nullptr, 0));
	}

	uint64_t to_uint64() const
	{
		return uint64_t(std::stoull(this->string, nullptr, 0));
	}

	float to_float() const
	{
		return std::stof(this->string);
	}

	double to_double() const
	{
		return std::stod(this->string);
	}

	long double to_long_double() const
	{
		return std::stold(this->string);
	}
};

} // namespace jsondom
