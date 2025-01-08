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

			sl::WindowCreateInfos createInfos {};
			createInfos.title = "Steelux";
			createInfos.size = {16*70, 9*70};
			if (m_window.create(createInfos) != sl::Result::eSuccess)
				return sl::Result::eFailure;

			VkApplicationInfo applicationInfos {};
			applicationInfos.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfos.pEngineName = "Steelux";
			applicationInfos.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
			applicationInfos.pApplicationName = "Steelux_sandbox";
			applicationInfos.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
			applicationInfos.apiVersion = VK_API_VERSION_1_3;

			VkInstanceCreateInfo instanceCreateInfos {};
			instanceCreateInfos.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfos.enabledLayerCount = 0;
			instanceCreateInfos.enabledExtensionCount = 0;
			instanceCreateInfos.pApplicationInfo = &applicationInfos;

			if (vkCreateInstance(&instanceCreateInfos, nullptr, &m_instance) != VK_SUCCESS)
				return sl::Result::eFailure;

			sl::UUID uuid1 {sl::UUID::generate()};
			sl::UUID uuid2 {sl::UUID::generate()};

			using namespace sl::literals;
			sl::EventFilter filter1 {
				.categories = {"abc"_ecat},
				.excludeCategories = {"def"_ecat},
				.sources = {},
				.excludeSources = {uuid1}
			};
			listener1 = sl::EventManager::addListener<sl::String> (filter1, [](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<sl::String> &event) -> void {
				sl::mainLogger.info("From listener1 : {}", event.data);
			});

			sl::EventManager::send<sl::String> ({"abc"_ecat}, sl::UUID(), {"Hello1"});
			sl::EventManager::send<sl::String> ({}, sl::UUID(), {"Hello_no_arrive1"});
			sl::EventManager::send<sl::String> ({"abc"_ecat}, uuid1, {"Hello_no_arrive2"});
			sl::EventManager::send<sl::String> ({"abc"_ecat}, uuid2, {"Hello2"});
			sl::EventManager::send<sl::String> ({"abc"_ecat, "def"_ecat}, sl::UUID(), {"Hello_no_arrive3"});


			while (sl::InputManager::update());

			return sl::Result::eSuccess;
		}

		auto onDestruction() noexcept -> void override {
			sl::EventManager::removeListener<sl::String> (listener1);
			vkDestroyInstance(m_instance, nullptr);
			std::println("Destruction");
		}

	private:
		sl::ListenerUUID listener1;
		sl::Window m_window;
		VkInstance m_instance;
};


auto createApplication() noexcept -> std::unique_ptr<sl::Application> {
	return std::make_unique<SandboxApp> ();
}
