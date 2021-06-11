#include <tst/set.hpp>
#include <tst/check.hpp>

#include "../../src/jsondom/dom.hpp"

#include <utki/debug.hpp>

namespace{
tst::set set("basic", [](tst::suite& suite){
	suite.add(
		"parse_json_from_string",
		[](){
			auto json = jsondom::read(R"qwertyuiop(
				{
					"name1": null,
					"name2" : ["hello", "world", false, true, null, 13]
				}
			)qwertyuiop");

			tst::check(json.is<jsondom::value_type::object>(), SL) << "actual = " << unsigned(json.type());
			tst::check_eq(json.object().size(), size_t(2), SL);
			tst::check(json.object()["name1"].is<jsondom::value_type::null>(), SL);
			tst::check(json.object()["name2"].is<jsondom::value_type::array>(), SL);

			auto& arr = json.object()["name2"].array();

			tst::check_eq(arr.size(), size_t(6), SL);
			tst::check(arr[0].is<jsondom::value_type::string>(), SL);
			tst::check_eq(arr[0].string(), std::string("hello"), SL);
			tst::check(arr[1].is<jsondom::value_type::string>(), SL);
			tst::check_eq(arr[1].string(), std::string("world"), SL);
			tst::check(arr[2].is<jsondom::value_type::boolean>(), SL);
			tst::check_eq(arr[2].boolean(), false, SL);
			tst::check(arr[3].is<jsondom::value_type::boolean>(), SL);
			tst::check_eq(arr[3].boolean(), true, SL);
			tst::check(arr[4].is<jsondom::value_type::null>(), SL);
			tst::check(arr[5].is<jsondom::value_type::number>(), SL);
			tst::check_eq(arr[5].number().to_int32(), 13, SL);
		}
	);
});
}
