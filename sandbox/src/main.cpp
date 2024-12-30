#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>

#include <memory>
#include <print>



struct SBO {
	char8_t *start;
	std::size_t length;
	std::size_t capacity;
};

using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			sl::memory::DebugAllocator<char> _ {};

			sl::String str1 {"Hello World from Steelux !"};
			sl::String str2 {str1};
			sl::String str3 {std::move(str1)};
			
			std::println("str1 : {} ({}, {})", (void*)str1.getData(), str1.getSize(), str1.getCapacity());
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			std::println("str3 : {} ({}, {})", str3.getData(), str2.getSize(), str2.getCapacity());
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
