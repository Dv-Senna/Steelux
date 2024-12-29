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
