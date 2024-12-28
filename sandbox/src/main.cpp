#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>

#include <memory>
#include <print>
#include <iostream>



struct SBO {
	char8_t *start;
	std::size_t length;
	std::size_t capacity;
};

using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			std::println("c8* : {}/{}, size_t : {}/{}, SBO : {}/{}", alignof(char8_t*), sizeof(char8_t*), alignof(std::size_t), sizeof(std::size_t), alignof(SBO), sizeof(SBO));
			m_infos.name = "Sandbox";

			std::println("std::string : {}/{}, sl::utils::String : {}/{}", alignof(std::string), sizeof(std::string), alignof(sl::utils::String), sizeof(sl::utils::String));

			sl::memory::DebugAllocator<char> debugAllocator {};

			sl::utils::String test {"ABCDEFGHIJKLMNOPQRSTUVW"};
			const sl::utils::String test2 {std::move(test)};
			sl::utils::String test3 {test2};
			std::println("test : {}", (std::size_t)test.getData());
			std::println("test2 : {}", test2);
			std::println("test3 : {}", test3);
			test3[5] = '_';
			std::println("test3 : {}", test3);

			for (auto &c : test3) {
				++c;
			}

			std::println("test3 : {}", test3);
			test3.pushBack('Z');
			test3.insert(test3.begin() + 5, '$');
			std::println("INSERT test3 : {}", test3);

			test3.popFront();
			test3.erase(1, 3);
			std::println("ERASE test3 : {}", test3);

			test3[-1] = '@';
			std::println("test3 : {}", test3);

			test3 += test2;
			std::println("BACK_CONCAT test3 : {}", test3);

			sl::utils::String test4 {test2 + " Hello " + test3 + " World !"};
			std::println("test4 : {}", test4);
			std::println("inline concat : {}", test2 + " Hello " + test3 + " World !");

			std::cout << "Some test with iostream : " << test2 << std::endl;
			std::cout << "Second test but with csv " << (test2 + test3) << std::endl;

//			std::println("stoi : {}", sl::utils::stringToNumber<int> ("10").value_or(std::numeric_limits<int>::max()));

			std::println("severity : {:5}", sl::utils::LogSeverity::eWarn | sl::utils::LogSeverity::eInfo);
			std::println("severity2 : {:4}", ~sl::utils::LogSeverity::eWarn);
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
