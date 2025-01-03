#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>
#include <sl/utils/units.hpp>

#include <sl/memory/poolAllocator.hpp>
#include <sl/memory/heapAllocator.hpp>

#include <memory>
#include <print>
#include <ranges>



using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			sl::memory::DebugAllocator<char> _ {};
			sl::memory::HeapAllocator heapAllocator {};

			using String = sl::utils::BasicString<char, sl::memory::HeapAllocatorView<char>>;


			std::println("sl::String : {}/{}", alignof(sl::String), sizeof(sl::String));
			std::println("std::string : {}/{}", alignof(std::string), sizeof(std::string));
			std::println("std::shared_ptr<char> {}/{}", alignof(std::shared_ptr<char>), sizeof(std::shared_ptr<char>));

			String str1 {"Hello World from Steelux !", sl::memory::HeapAllocatorView<char> (heapAllocator)};
			String str2 {str1};
			const String str3 {std::move(str1)};
			
			std::println("str1 : {} ({}, {})", (void*)str1.getData(), str1.getSize(), str1.getCapacity());
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			std::println("str3 : {} ({}, {})", str3, str2.getSize(), str2.getCapacity());

			str2[-1] = str3[3];
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			for (const auto &c : str3)
				std::println("c : {}", c);
			for (auto &c : str2)
				++c;
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			for (auto &c : str2)
				--c;

			std::println("------------ START OF INSERTION -------------");
			str2.pushFront('*', 10);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.pushBack('-', 3);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 7, '+', 5);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			std::println("------------ START OF ERASION -------------");
			str2.erase(str2.begin() + 7, 5);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.popBack(3);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.popFront(10);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			std::println("------------ SHRINK TO FIT -------------");
			str2.shrinkToFit();
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.shrinkToFit();

			std::println("------------ INSERTION OF OTHER STRING -------------");
			str2.pushFront("- Hi. - ");
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.pushBack(". Idk what to say");
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.insert(str2.rbegin() + 17, ". Ooh");
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			str2 = str3;
			str2.shrinkToFit();

			std::println("------------ INSERTION OF ITERATOR RANGES -------------");
			std::string alphabet {"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
			str2.pushFront(alphabet.begin(), alphabet.begin() + 4);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.pushBack(alphabet.begin() + 10, alphabet.begin() + 14);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 4, alphabet.begin() + 4, alphabet.begin() + 8);
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			str2 = str3;
			str2.shrinkToFit();

			std::println("------------ INSERTION OF RANGES -------------");
			str2.pushFront(std::views::iota(0, 4) | std::views::transform([](auto val) -> char {return 'A' + val;}));
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.pushBack(std::views::iota(10, 14) | std::views::transform([](auto val) -> char {return 'A' + val;}));
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 4, std::views::iota(4, 8) | std::views::transform([](auto val) -> char {return 'A' + val;}));
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());


			std::println("------------ CONCAT STRING VIEW -------------");
			using Test = sl::utils::ConcatStringView<char(&)[4], sl::String, const char*>;
			static_assert(std::same_as<Test::FirstString, sl::String>);
			static_assert(std::same_as<Test::Tuple, std::tuple<char(&)[4], sl::String, const char*>>);
			static_assert(std::same_as<Test::AddressTuple, std::tuple<const char*, const sl::String*, const char*>>);

			sl::String csv_str1 {"Hello World !"};
			String csv_str2 {"Something ?", sl::memory::HeapAllocatorView<char> (heapAllocator)};
			str2 = csv_str2 + "Hello" + csv_str1 + "Hi!" + "IDK what I'm doing";
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());
			str2 += ". Heeeeeeeere we gooo";
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());


			std::println("------------ STRING ENUMS -------------");
			std::println("sl::Result::eSuccess : {}", sl::Result::eSuccess);
			std::println("sl::Result::eFailure : {}", sl::Result::eFailure);

			std::println("------------ BIT FLAG ENUMS -------------");
			std::println("~sl::utils::LogSeverity::eWarn : {}", ~sl::utils::LogSeverity::eWarn);
			std::println("~sl::utils::LogSeverity::eWarn<4> : {:4}", ~sl::utils::LogSeverity::eWarn);
			std::println("sl::utils::LogSeverity::eWarn | sl::utils::LogSeverity::eError : {}", sl::utils::LogSeverity::eWarn | sl::utils::LogSeverity::eError);

			str2 = str3;
			str2.shrinkToFit();

			std::println("------------ LOGGER -------------");
			//sl::mainLogger.setSeverityMask(~sl::utils::LogSeverity::eDebug);
			sl::mainLogger.debug("Debug {}", str2);
			sl::mainLogger.info("Info {}", str2);
			sl::mainLogger.warn("Warning {}", str2);
			sl::mainLogger.error("Error {}", str2);


			std::println("------------ UNITS -------------");
			sl::mainLogger.info("1Mio : {}, {}, {}", 1_MiB, sl::utils::Kibibytes(1_MiB), sl::utils::Bytes(1_MiB));


			std::println("------------ POOL ALLOCATOR -------------");
			sl::memory::PoolAllocator<int> poolAllocator {64};
			int *value {poolAllocator.allocate()};
			*value = 10;
			sl::mainLogger.info("value : {}, {}", (void*)value, *value);
			for (const auto &_ : std::views::iota(0, 30))
				(void)poolAllocator.allocate();
			//poolAllocator.deallocate(value);
			value = poolAllocator.allocate();
			sl::mainLogger.info("value : {}, {}", (void*)value, *value);
			poolAllocator.deallocate(value - 10);
			for (const auto &allocation : poolAllocator)
				sl::mainLogger.info("\t- Allocation : {}, {}", (void*)&allocation, allocation);
		}

		~SandboxApp() override {

		}

		auto onCreation() noexcept -> sl::Result override {
			std::println("Creation");
			return sl::Result::eSuccess;
		}

		auto onDestruction() noexcept -> void override {
			std::println("Destruction");
		}
};


auto createApplication() noexcept -> std::unique_ptr<sl::Application> {
	return std::make_unique<SandboxApp> ();
}
