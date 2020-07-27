#pragma once

#include <stdexcept>

namespace jsondom{

/**
 * @brief Malformed JSON error.
 * This exception is thrown during JSON parsing in case
 * unexpected characters are encountered.
 */
class malformed_json_error : public std::runtime_error{
public:
	malformed_json_error(const std::string& message) :
			std::runtime_error(message)
	{}
};

}
