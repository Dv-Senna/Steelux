#include <algorithm>

#include <catch2/catch_test_macros.hpp>

#include <sl/utils/array.hpp>


TEST_CASE("sl::Array<T, D>", "[sl::Array]") {
	sl::Array<int, 10> array1 {};
	sl::Array<int, 10> array2 {3};
	sl::Array<int, 10> array3 {1, 2, 3, 4, 5, 6};
	sl::Array<int, 10> array4 {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

	REQUIRE(array1 == array1);
	REQUIRE(array2 == array2);
	REQUIRE(array3 == array3);
	REQUIRE(array4 == array4);

	REQUIRE(array1 != array2);
	REQUIRE(array1 != array3);
	REQUIRE(array1 != array4);
	REQUIRE(array2 != array3);
	REQUIRE(array2 != array4);
	REQUIRE(array3 != array4);

	REQUIRE(array1 == sl::Array<int, 10> (0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
	REQUIRE(array2 == sl::Array<int, 10> (3, 3, 3, 3, 3, 3, 3, 3, 3, 3));
	REQUIRE(array3 == sl::Array<int, 10> (1, 2, 3, 4, 5, 6, 0, 0, 0, 0));
	REQUIRE(array4 == sl::Array<int, 10> (1, 2, 3, 4, 5, 6, 7, 8, 9, 10));

	array1 = array2;
	REQUIRE(array1 == sl::Array<int, 10> (3, 3, 3, 3, 3, 3, 3, 3, 3, 3));
	array3 = std::move(array2);
	REQUIRE(array3 == sl::Array<int, 10> (3, 3, 3, 3, 3, 3, 3, 3, 3, 3));

	std::size_t i {0};
	std::ranges::transform(array2, array2.begin(), [&i](int val) -> int {return val + (i++) - 2;});
	REQUIRE(array2 == array4);
}
