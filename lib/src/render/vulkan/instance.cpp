#include "sl/render/vulkan/instance.hpp"

#include "sl/utils/errorStack.hpp"


namespace sl::render::vulkan {
	auto Instance::create(const InstanceCreateInfos &createInfos) noexcept -> sl::Result {
		m_window = createInfos.window;

		VkApplicationInfo appInfos {};
		appInfos.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfos.apiVersion = VK_API_VERSION_1_3;
		appInfos.pEngineName = "Steelux";
		appInfos.engineVersion = VK_MAKE_API_VERSION(0, SL_VERSION_MAJOR, SL_VERSION_MINOR, SL_VERSION_PATCH);
		appInfos.pApplicationName = createInfos.appName.getData();
		appInfos.applicationVersion = VK_MAKE_API_VERSION(0, createInfos.appVersion.major, createInfos.appVersion.minor, createInfos.appVersion.patch);

		VkInstanceCreateInfo instanceCreateInfos {};
		instanceCreateInfos.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfos.pApplicationInfo = &appInfos;
		instanceCreateInfos.enabledExtensionCount = 0;
		instanceCreateInfos.ppEnabledExtensionNames = nullptr;
		instanceCreateInfos.enabledLayerCount = 0;
		instanceCreateInfos.ppEnabledLayerNames = nullptr;

		if (vkCreateInstance(&instanceCreateInfos, nullptr, &m_instance) != VK_SUCCESS)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create vulkan instance");

		return sl::Result::eSuccess;
	}


	auto Instance::destroy() noexcept -> void {
		if (m_instance != VK_NULL_HANDLE)
			vkDestroyInstance(m_instance, nullptr);
	}

} // namespace sl::render::vulkan
