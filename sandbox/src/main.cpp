#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <vulkan/vulkan.h>

#include <sl/utils/array.hpp>
#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>
#include <sl/utils/units.hpp>
#include <sl/utils/hash.hpp>

#include <sl/memory/poolAllocator.hpp>
#include <sl/memory/heapAllocator.hpp>
#include <sl/memory/stackAllocator.hpp>
#include <sl/memory/doubleStackAllocator.hpp>
#include <sl/memory/singleFrameAllocator.hpp>
#include <sl/memory/doubleBufferedAllocator.hpp>

#include <sl/window.hpp>
#include <sl/eventManager.hpp>
#include <sl/inputManager.hpp>

#include <memory>
#include <print>



using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			m_infos.name = "Steelux_sandbox";
			m_infos.title = "Steelux sandbox app";
			m_infos.version = "0.1.0"_ver;
			m_infos.fps = 60.0_hz;

			sl::memory::HeapAllocator heapAllocator {};
			sl::memory::HeapMemoryResource heapMemoryResource {heapAllocator};

			std::println("------------ PMR STRING -------------");
			sl::utils::BasicString<char, std::pmr::polymorphic_allocator<char>> str {"Hello World from Steelux !", {&heapMemoryResource}};
			std::println("str : {} ({}, {})", str, str.getSize(), str.getCapacity());

			sl::utils::BasicString<char, sl::memory::HeapAllocatorView<char>> str2 {"Hello World for a 2nd string from Steelux !", {heapAllocator}};
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			std::println("------------ HASH -------------");
			std::println("'Hello World !'_hash8 : {}", "Hello World !"_hash8);
			std::println("'Hello World !'_hash16 : {}", "Hello World !"_hash16);
			std::println("'Hello World !'_hash32 : {}", "Hello World !"_hash32);
			std::println("'Hello World !'_hash64 : {}", "Hello World !"_hash64);

			sl::String str3 {"Hello World !"};
			std::println("hash8(str) : {}", sl::utils::hash<sl::utils::Hash8> (str3));
			std::println("hash16(str) : {}", sl::utils::hash<sl::utils::Hash16> (str3));
			std::println("hash32(str) : {}", sl::utils::hash<sl::utils::Hash32> (str3));
			std::println("hash64(str) : {}", sl::utils::hash<sl::utils::Hash64> (str3));

		}

		~SandboxApp() override {

		}

		auto onCreation() noexcept -> sl::Result override {
			std::println("Creation");

			return sl::Result::eSuccess;
		}


		auto onUpdate(sl::utils::Millisecond /*dt*/) noexcept -> std::expected<bool, sl::Result> override {
			//sl::mainLogger.info("DT : {}", dt);
			if (sl::InputManager::isKeyDown(sl::Key::eEscape))
				return false;
			return true;
		}


		auto onDestruction() noexcept -> void override {
			std::println("Destruction");
		}

	private:
};


auto createApplication() noexcept -> std::unique_ptr<sl::Application> {
	return std::make_unique<SandboxApp> ();
}
