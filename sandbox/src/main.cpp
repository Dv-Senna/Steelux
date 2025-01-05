#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/array.hpp>
#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>
#include <sl/utils/units.hpp>

#include <sl/memory/poolAllocator.hpp>
#include <sl/memory/heapAllocator.hpp>
#include <sl/memory/stackAllocator.hpp>
#include <sl/memory/doubleStackAllocator.hpp>
#include <sl/memory/singleFrameAllocator.hpp>
#include <sl/memory/doubleBufferedAllocator.hpp>

#include <memory>
#include <print>



using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			std::println("------------ ARRAY -------------");
			sl::utils::Array<int, 10> array {3};
			sl::utils::Array<int, 10> array2 {array};
			sl::utils::Array<int, 10> array3 {std::move(array)};
			std::println("array : {}", array);
			std::println("array2 : {}", array2);
			std::println("array3 : {}", array3);

			std::size_t i {0};
			std::ranges::transform(array, array.begin(), [&i](int val) {return val + (i++);});
			std::println("array : {}", array);
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
