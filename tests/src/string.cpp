#include <catch2/catch_test_macros.hpp>

#include <sl/utils/string.hpp>


TEST_CASE("Test of sl::String", "[sl::String]") {
	sl::String str {"Hello World !"};

	REQUIRE(str == str);
	REQUIRE(str.getSize() == 13);
	REQUIRE(str.getCapacity() == 14);
}
