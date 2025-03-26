#include <regex>

#include <tst/set.hpp>
#include <tst/check.hpp>

#include <papki/vector_file.hpp>
#include <papki/fs_file.hpp>

#include "../../src/jsondom/dom.hpp"

namespace{
const std::string data_dir = "samples_data/";
}

namespace{
const tst::set set("samples", [](tst::suite& suite){
    std::vector<std::string> files;

    {
        const std::regex suffix_regex("^.*\\.json$");
        auto all_files = papki::fs_file(data_dir).list_dir();

        std::copy_if(
                all_files.begin(),
                all_files.end(),
                std::back_inserter(files),
                [&suffix_regex](auto& f){
                    return std::regex_match(f, suffix_regex);
                }
            );
    }

    suite.add<std::string>(
        "sample",
        std::move(files),
        [](const auto& p){
            auto in_file_name = data_dir + p;

            auto json = jsondom::read(papki::fs_file(in_file_name));

            papki::vector_file out_file;
            jsondom::write(out_file, json);
            
            auto out_data = out_file.reset_data();

            auto cmp_data = papki::fs_file(in_file_name + ".cmp").load();

            if(out_data != cmp_data){
                papki::fs_file failed_file(p + ".out");

                {
                    papki::file::guard file_guard(failed_file, papki::mode::create);
                    failed_file.write(out_data);
                }

                tst::check(false, SL) << "parsed file is not as expected: " << in_file_name;
            }
        }
    );
});
}