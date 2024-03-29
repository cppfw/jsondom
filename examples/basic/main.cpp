#include <iostream>
#include <cassert>

#include <jsondom/dom.hpp>

#include <utki/debug.hpp>

#ifdef assert
#	undef assert
#endif

constexpr const char* json = R"qwertyuiop(
	{
		"key1": "value1",
		"key2": "value2"
	}
)qwertyuiop";

int main(int c, const char** v){
	// parse the JSON to DOM
	auto dom = jsondom::read(json);

	// check that root node is a JSON object, just to be sure
	utki::assert(dom.is_object(), SL);

	// let's check if there is an expected key1-value1 pair,
	// but do not fail if there is no such key or value is not of an expected type

	auto value1_i = dom.object().find("key1");
	if(value1_i != dom.object().end()){
		auto& value1 = value1_i->second;
		if(value1.is_string()){
			std::cout << "value1 = " << value1.string() << std::endl;
		}else{
			// this never happens in this example, but just to show how
			// to handle cases when JSON value type is not as expected
			std::cout << "value1 is not a string" << std::endl;
		}
	}else{
		// this never happens in this example, but just to show how
		// to handle cases when JSON key is not found
		std::cout << "key1 is not found" << std::endl;
	}

	// Try to use key2-value2 pair, relying on that it exists.
	// An exception will be thrown if something goes wrong, like
	// no 'key2' key is found or value is of unexpected type.

	try{
		std::cout << "value2 = " << dom.object().at("key2").string() << std::endl;
	}catch(std::logic_error& e){
		// handle exception here if needed
		std::cout << e.what() << std::endl;
		throw;
	}

	return 0;
}
