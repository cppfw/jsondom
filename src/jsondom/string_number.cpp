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

#include "string_number.hpp"

#include <array>

using namespace jsondom;

string_number::string_number(int value) :
	string([](int value) -> std::string {
		// TRACE(<< "string_number::string_number(int): value = " << value << std::endl)
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%d", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(unsigned char value) :
	string_number((unsigned short int)value)
{}

string_number::string_number(unsigned short int value) :
	string_number((unsigned int)value)
{}

string_number::string_number(unsigned int value) :
	string([](unsigned int value) -> std::string {
		// TRACE(<< "string_number::string_number(uint): value = " << value <<", base = " << int(conversion_base) <<
		// std::endl)
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%u", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(signed long int value) :
	string([](long int value) -> std::string {
		// TRACE(<< "string_number::string_number(long int): value = " << value << std::endl)
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%ld", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(unsigned long int value) :
	string([](unsigned long int value) -> std::string {
		// TRACE(<< "string_number::string_number(ulong): value = " << value << ", base = " << int(conversion_base) <<
		// std::endl)
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%lu", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(signed long long int value) :
	string([](long long int value) -> std::string {
		// TRACE(<< "string_number::string_number(long long): value = " << value << std::endl)
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%lld", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(unsigned long long int value) :
	string([](unsigned long long int value) -> std::string {
		// TRACE(<< "string_number::string_number(u long long): value = " << value << ", base = " <<
		// int(conversion_base) << std::endl)
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%llu", value);

		if (0 <= res && res <= int(buf.size())) {
			return {buf.data(), size_t(res)};
		}
		return {};
	}(value))
{}

string_number::string_number(float value) :
	string([](float value) -> std::string {
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%.8G", double(value));

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

string_number::string_number(double value) :
	string([](double value) -> std::string {
		std::array<char, 64> buf{}; // NOLINT

		// TODO: use something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%.17G", value);

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}

string_number::string_number(long double value) :
	string([](long double value) -> std::string {
		constexpr auto buf_len = 128;
		std::array<char, buf_len> buf{};

		// TODO: use std::to_string() or something else
		// NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg)
		int res = snprintf(buf.data(), buf.size(), "%.31LG", value);

		if (res < 0 || res > int(buf.size())) {
			return {};
		} else {
			return {buf.data(), size_t(res)};
		}
	}(value))
{}
