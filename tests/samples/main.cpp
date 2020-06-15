#include "../../src/jsondom/dom.hpp"

#include <papki/fs_file.hpp>
#include <clargs/parser.hpp>


int main(int argc, char** argv){
	bool help = false;
	std::string out_filename = "out.json";
	std::string in_filename;

	{
		clargs::parser p;

		p.add("help", "show help information", [&help](){help = true;});
		p.add('o', "out-file", "output filename", [&out_filename](std::string&& f){out_filename = std::move(f);});

		auto extras = p.parse(argc, argv);

		if(help){
			std::cout << "JSON parser test utility. Parses JSON into DOM and then writes out the DOM to JSON file." << std::endl;
			std::cout << "Usage:" << std::endl;
			std::cout << "  " << argv[0] << " [-o <out-json-filename>] <in-json-filename>" << std::endl;
			std::cout << "Arguments:" << std::endl;
			std::cout << p.description() << std::endl;
			return 0;
		}

		if(extras.empty()){
			std::cout << "error: no input filename given" << std::endl;
			return 1;
		}
		in_filename = extras.front();
	}

	auto json = jsondom::read(papki::fs_file(in_filename));

	jsondom::write(papki::fs_file(out_filename), json);

	return 0;
}
