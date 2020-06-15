#pragma once

#include <stdexcept>

namespace jsondom{

class malformed_json_error : public std::runtime_error{
public:
	malformed_json_error(const std::string& message) :
			std::runtime_error(message)
	{}
};

}
