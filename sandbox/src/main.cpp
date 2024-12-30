#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>

#include <memory>
#include <print>
#include <ranges>



using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			sl::memory::DebugAllocator<char> _ {};

			std::println("sl::String : {}/{}", alignof(sl::String), sizeof(sl::String));
			std::println("std::string : {}/{}", alignof(std::string), sizeof(std::string));
			std::println("std::shared_ptr<char> {}/{}", alignof(std::shared_ptr<char>), sizeof(std::shared_ptr<char>));

			sl::String str1 {"Hello World from Steelux !"};
			sl::String str2 {str1};
			const sl::String str3 {std::move(str1)};
			
			std::println("str1 : {} ({}, {})", (void*)str1.getData(), str1.getSize(), str1.getCapacity());
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			std::println("str3 : {} ({}, {})", str3.getData(), str2.getSize(), str2.getCapacity());

			str2[-1] = str3[3];
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());

			for (const auto &c : str3)
				std::println("c : {}", c);
			for (auto &c : str2)
				++c;
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			for (auto &c : str2)
				--c;

			std::println("------------ START OF INSERTION -------------");
			str2.pushFront('*', 10);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.pushBack('-', 3);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 7, '+', 5);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());

			std::println("------------ START OF ERASION -------------");
			str2.erase(str2.begin() + 7, 5);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.popBack(3);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.popFront(10);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());

			std::println("------------ SHRINK TO FIT -------------");
			str2.shrinkToFit();
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.shrinkToFit();

			std::println("------------ INSERTION OF OTHER STRING -------------");
			str2.pushFront("- Hi. - ");
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.pushBack(". Idk what to say");
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.insert(str2.rbegin() + 17, ". Ooh");
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());

			str2 = str3;
			str2.shrinkToFit();

			std::println("------------ INSERTION OF ITERATOR RANGES -------------");
			std::string alphabet {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
			str2.pushFront(alphabet.begin(), alphabet.begin() + 4);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.pushBack(alphabet.begin() + 10, alphabet.begin() + 14);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 4, alphabet.begin() + 4, alphabet.begin() + 8);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());

			str2 = str3;
			str2.shrinkToFit();

			std::println("------------ INSERTION OF ITERATOR RANGES -------------");
			str2.pushFront(std::views::iota(0, 4) | std::views::transform([](auto val) -> char {return 'A' + val;}));
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.pushBack(std::views::iota(10, 14) | std::views::transform([](auto val) -> char {return 'A' + val;}));
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 4, std::views::iota(4, 8) | std::views::transform([](auto val) -> char {return 'A' + val;}));
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
		}

		~SandboxApp() override {

		}

		sl::Result onCreation() noexcept override {
			std::println("Creation");
			return sl::Result::eSuccess;
		}

		void onDestruction() noexcept override {
			std::println("Destruction");
		}
};


std::unique_ptr<sl::Application> createApplication() {
	return std::make_unique<SandboxApp> ();
}
