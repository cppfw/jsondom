#include <jsondom/dom.hpp>

int main(int argc, const char** argv){
    auto json = jsondom::read(R"qwe({"hello":"world"})qwe");

    std::cout << "json = " << json.to_string() << std::endl;

    return 0;
}
