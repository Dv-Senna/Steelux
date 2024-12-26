#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>

using namespace sl::utils::literals;

#include <memory>
#include <print>



class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			m_infos.name = "Sandbox";

			std::println("eSuccess : {}", sl::utils::toString(sl::Result::eSuccess));
			std::println("eFailure : {}", sl::utils::toString(sl::Result::eFailure));
			std::println("eAllocationFailure : {}", sl::utils::toString(sl::Result::eAllocationFailure));
			std::println("eFileFailure : {}", sl::utils::toString(sl::Result::eFileFailure));
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
