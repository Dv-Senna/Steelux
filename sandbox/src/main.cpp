#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>

#include <memory>
#include <print>



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

			str2.pushFront('*', 10);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.pushBack('-', 3);
			std::println("str2 : {} ({}, {})", str2.getData(), str2.getSize(), str2.getCapacity());
			str2.insert(str2.begin() + 7, '+', 5);
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
