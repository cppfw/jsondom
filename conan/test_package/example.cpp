#include <jsondom/dom.hpp>

#include <iostream>

int main(int argc, const char** argv){

    auto json = jsondom::read(R"qwertyuiop(
        {
            "name1": null,
            "name2" : ["hello", "jsondom", false, true, null, 13]
        }
    )qwertyuiop");

    auto& arr = json.object()["name2"].array();

	std::cout << "Hello " << arr[1].string() <<  "!" << '\n';

	return 0;
}
