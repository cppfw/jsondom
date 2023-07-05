#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/jsondom/dom.hpp"

#include <utki/debug.hpp>

using namespace std::string_literals;

namespace{
const tst::set set("basic", [](tst::suite& suite){
	suite.add(
		"parse_json_from_string",
		[](){
			auto json = jsondom::read(R"qwertyuiop(
				{
					"name1": null,
					"name2" : ["hello", "world", false, true, null, 13]
				}
			)qwertyuiop");

			tst::check(json.is<jsondom::type::object>(), SL) << "actual = " << unsigned(json.get_type());
			tst::check_eq(json.object().size(), size_t(2), SL);
			tst::check(json.object()["name1"].is<jsondom::type::null>(), SL);
			tst::check(json.object()["name2"].is<jsondom::type::array>(), SL);

			auto& arr = json.object()["name2"].array();

			tst::check_eq(arr.size(), size_t(6), SL);
			tst::check(arr[0].is<jsondom::type::string>(), SL);
			tst::check_eq(arr[0].string(), std::string("hello"), SL);
			tst::check(arr[1].is<jsondom::type::string>(), SL);
			tst::check_eq(arr[1].string(), std::string("world"), SL);
			tst::check(arr[2].is<jsondom::type::boolean>(), SL);
			tst::check_eq(arr[2].boolean(), false, SL);
			tst::check(arr[3].is<jsondom::type::boolean>(), SL);
			tst::check_eq(arr[3].boolean(), true, SL);
			tst::check(arr[4].is<jsondom::type::null>(), SL);
			tst::check(arr[5].is<jsondom::type::number>(), SL);
			tst::check_eq(arr[5].number().to_int32(), 13, SL);
		}
	);

	suite.add("value_to_string", [](){
		auto json = jsondom::read(R"qwertyuiop(
				{
					"name1": null,
					"name2" : ["hello", "world", false, true, null, 13]
				}
			)qwertyuiop");
		
		auto str = json.to_string();

		tst::check_eq(
			str,
			R"({"name1":null,"name2":["hello","world",false,true,null,13]})"s,
			SL
		);
	});
});
}
