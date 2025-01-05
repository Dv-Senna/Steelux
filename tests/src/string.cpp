#include <ranges>
#include <string>

#include <catch2/catch_test_macros.hpp>

#include <sl/utils/string.hpp>


TEST_CASE("sl::String : SSO", "[sl::String]") {
	#define STR_LITERAL "Hello World !"
	const char strLiteral[] {STR_LITERAL};
	sl::String str {strLiteral};

	REQUIRE(str == str);
	REQUIRE(str == strLiteral);
	REQUIRE(str != "Hello World .");
	REQUIRE(str.getSize() == 13);
	REQUIRE(str.getCapacity() == 16);

	SECTION("Random access and iteration") {
		for (std::size_t i {0}; i < str.getSize(); ++i)
			REQUIRE(str[i] == strLiteral[i]);
		for (std::size_t i {0}; i < str.getSize(); ++i) {
			++str[i];
			REQUIRE(str[i] == strLiteral[i] + 1);
			--str[i];
		}

		REQUIRE(str[-1] == strLiteral[str.getSize() - 1]);

		std::size_t i {0};
		for (const auto &c : str) {
			REQUIRE(c == strLiteral[i]);
			++i;
		}
	}

	SECTION("Value insertion and erasion") {
		str.pushFront('*', 10);
		REQUIRE(str == "**********" STR_LITERAL);
		str.pushBack('-', 3);
		REQUIRE(str == "**********" STR_LITERAL "---");
		str.insert(str.begin() + 7, '+', 5);
		REQUIRE(str == "*******+++++***" STR_LITERAL "---");

		str.erase(str.begin() + 7, 5);
		REQUIRE(str == "**********" STR_LITERAL "---");
		str.popBack(3);
		REQUIRE(str == "**********" STR_LITERAL);
		str.popFront(10);
		REQUIRE(str == strLiteral);

		REQUIRE(str.getCapacity() == 36);
		str.shrinkToFit();
		REQUIRE(str.getCapacity() == 16);
	}

	SECTION("String insertion") {
		str.pushFront("- Hi. - ");
		REQUIRE(str == "- Hi. - " STR_LITERAL);
		str.pushBack(". Idk what to say");
		REQUIRE(str == "- Hi. - " STR_LITERAL ". Idk what to say");
		str.insert(str.rbegin() + 17, ". Ooh");
		REQUIRE(str == "- Hi. - " STR_LITERAL ". Ooh. Idk what to say");
	}

	SECTION("Iterator range insertion") {
		std::string alphabet {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
		str.pushFront(alphabet.begin(), alphabet.begin() + 4);
		REQUIRE(str == "ABCD" STR_LITERAL);
		str.pushBack(alphabet.begin() + 10, alphabet.begin() + 14);
		REQUIRE(str == "ABCD" STR_LITERAL "KLMN");
		str.insert(str.begin() + 4, alphabet.begin() + 4, alphabet.begin() + 8);
		REQUIRE(str == "ABCDEFGH" STR_LITERAL "KLMN");
	}

	SECTION("Range insertion") {
		str.pushFront(std::views::iota(0, 4) | std::views::transform([](auto val) -> char {return 'A' + val;}));
		REQUIRE(str == "ABCD" STR_LITERAL);
		str.pushBack(std::views::iota(10, 14) | std::views::transform([](auto val) -> char {return 'A' + val;}));
		REQUIRE(str == "ABCD" STR_LITERAL "KLMN");
		str.insert(str.begin() + 4, std::views::iota(4, 8) | std::views::transform([](auto val) -> char {return 'A' + val;}));
		REQUIRE(str == "ABCDEFGH" STR_LITERAL "KLMN");
	}

	SECTION("Concat string view") {
		sl::String csv_str1 {"Hello World !"};
		sl::String csv_str2 {"Something ?"};
		str = csv_str2 + "Hello" + csv_str1 + "Hi!" + "IDK what I'm doing";
		REQUIRE(str == "Something ?HelloHello World !Hi!IDK what I'm doing");
		str += ". Heeeeeeeere we goooo";
		REQUIRE(str == "Something ?HelloHello World !Hi!IDK what I'm doing. Heeeeeeeere we goooo");
	}

	#undef STR_LITERAL
}





TEST_CASE("sl::String : Heap", "[sl::String]") {
	#define STR_LITERAL "Hello World from Steelux !"
	const char strLiteral[] {STR_LITERAL};
	sl::String str {strLiteral};

	REQUIRE(str == str);
	REQUIRE(str == strLiteral);
	REQUIRE(str != "Hello World from Steelux .");
	REQUIRE(str.getSize() == 26);
	REQUIRE(str.getCapacity() == 27);

	SECTION("Random access and iteration") {
		for (std::size_t i {0}; i < str.getSize(); ++i)
			REQUIRE(str[i] == strLiteral[i]);
		for (std::size_t i {0}; i < str.getSize(); ++i) {
			++str[i];
			REQUIRE(str[i] == strLiteral[i] + 1);
			--str[i];
		}

		REQUIRE(str[-1] == strLiteral[str.getSize() - 1]);

		std::size_t i {0};
		for (const auto &c : str) {
			REQUIRE(c == strLiteral[i]);
			++i;
		}
	}

	SECTION("Value insertion and erasion and shrinkToFit") {
		str.pushFront('*', 10);
		REQUIRE(str == "**********" STR_LITERAL);
		str.pushBack('-', 3);
		REQUIRE(str == "**********" STR_LITERAL "---");
		str.insert(str.begin() + 7, '+', 5);
		REQUIRE(str == "*******+++++***" STR_LITERAL "---");

		str.erase(str.begin() + 7, 5);
		REQUIRE(str == "**********" STR_LITERAL "---");
		str.popBack(3);
		REQUIRE(str == "**********" STR_LITERAL);
		str.popFront(10);
		REQUIRE(str == strLiteral);

		REQUIRE(str.getCapacity() == 60);
		str.shrinkToFit();
		REQUIRE(str.getCapacity() == 27);
	}

	SECTION("String insertion") {
		str.pushFront("- Hi. - ");
		REQUIRE(str == "- Hi. - " STR_LITERAL);
		str.pushBack(". Idk what to say");
		REQUIRE(str == "- Hi. - " STR_LITERAL ". Idk what to say");
		str.insert(str.rbegin() + 17, ". Ooh");
		REQUIRE(str == "- Hi. - " STR_LITERAL ". Ooh. Idk what to say");
	}

	SECTION("Iterator range insertion") {
		std::string alphabet {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
		str.pushFront(alphabet.begin(), alphabet.begin() + 4);
		REQUIRE(str == "ABCD" STR_LITERAL);
		str.pushBack(alphabet.begin() + 10, alphabet.begin() + 14);
		REQUIRE(str == "ABCD" STR_LITERAL "KLMN");
		str.insert(str.begin() + 4, alphabet.begin() + 4, alphabet.begin() + 8);
		REQUIRE(str == "ABCDEFGH" STR_LITERAL "KLMN");
	}

	SECTION("Range insertion") {
		str.pushFront(std::views::iota(0, 4) | std::views::transform([](auto val) -> char {return 'A' + val;}));
		REQUIRE(str == "ABCD" STR_LITERAL);
		str.pushBack(std::views::iota(10, 14) | std::views::transform([](auto val) -> char {return 'A' + val;}));
		REQUIRE(str == "ABCD" STR_LITERAL "KLMN");
		str.insert(str.begin() + 4, std::views::iota(4, 8) | std::views::transform([](auto val) -> char {return 'A' + val;}));
		REQUIRE(str == "ABCDEFGH" STR_LITERAL "KLMN");
	}

	SECTION("Concat string view") {
		sl::String csv_str1 {"Hello World !"};
		sl::String csv_str2 {"Something ?"};
		str = csv_str2 + "Hello" + csv_str1 + "Hi!" + "IDK what I'm doing";
		REQUIRE(str == "Something ?HelloHello World !Hi!IDK what I'm doing");
		str += ". Heeeeeeeere we goooo";
		REQUIRE(str == "Something ?HelloHello World !Hi!IDK what I'm doing. Heeeeeeeere we goooo");
	}

	#undef STR_LITERAL
}
