#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>

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
			std::println("c8* : {}/{}, size_t : {}/{}, SBO : {}/{}", alignof(char8_t*), sizeof(char8_t*), alignof(std::size_t), sizeof(std::size_t), alignof(SBO), sizeof(SBO));
			m_infos.name = "Sandbox";

			sl::utils::String test {"ABCDEFGHIJKLM"};
			sl::utils::String test2 {std::move(test)};
			std::println("test : {}", (std::size_t)test.getData());
			std::println("test2 : {}", test2.getData());
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
