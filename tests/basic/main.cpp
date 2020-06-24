#include "../../src/jsondom/dom.hpp"

#include <utki/debug.hpp>

int main(int argc, char** argv){
	// test json parsing from string
	{
		auto json = jsondom::read(R"qwertyuiop(
			{
				"name1": null,
				"name2" : ["hello", "world", false, true, null, 13]
			}
		)qwertyuiop");

		ASSERT_INFO_ALWAYS(json.is<jsondom::value_type::object>(), "actual = " << unsigned(json.type()))
		ASSERT_INFO_ALWAYS(json.object().size() == 2, "actual = " << json.object().size())
		ASSERT_ALWAYS(json.object()["name1"].is<jsondom::value_type::null>())
		ASSERT_ALWAYS(json.object()["name2"].is<jsondom::value_type::array>())

		auto& arr = json.object()["name2"].array();

		ASSERT_ALWAYS(arr.size() == 6)
		ASSERT_ALWAYS(arr[0].is<jsondom::value_type::string>())
		ASSERT_ALWAYS(arr[0].string() == "hello")
		ASSERT_ALWAYS(arr[1].is<jsondom::value_type::string>())
		ASSERT_ALWAYS(arr[1].string() == "world")
		ASSERT_ALWAYS(arr[2].is<jsondom::value_type::boolean>())
		ASSERT_ALWAYS(arr[2].boolean() == false)
		ASSERT_ALWAYS(arr[3].is<jsondom::value_type::boolean>())
		ASSERT_ALWAYS(arr[3].boolean() == true)
		ASSERT_ALWAYS(arr[4].is<jsondom::value_type::null>())
		ASSERT_ALWAYS(arr[5].is<jsondom::value_type::number>())
		ASSERT_ALWAYS(arr[5].number().to_int32() == 13)
	}

	return 0;
}
