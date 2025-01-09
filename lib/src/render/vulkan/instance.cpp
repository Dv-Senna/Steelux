#include "sl/render/vulkan/instance.hpp"

#include <ranges>
#include <vector>

#include <SDL3/SDL_vulkan.h>

#include "sl/utils/errorStack.hpp"
#include "sl/utils/logger.hpp"


namespace sl::render::vulkan {

#ifndef NDEBUG

	auto checkValidationLayerValidity(const char *layerName) noexcept -> bool {
		static std::vector<VkLayerProperties> layerProperties {};
		static bool initialized {false};

		if (initialized) {
			bool supported {std::ranges::find_if(layerProperties, [&layerName](const VkLayerProperties &properties) -> bool {
				return strcmp(properties.layerName, layerName) == 0;
			}) != layerProperties.end()};
			
			if (!supported)
				sl::mainLogger.warn("Validation layer '{}' is not supported. It will not be enabled", layerName);

			return supported;
		}

		initialized = true;
		std::uint32_t supportedLayerCount {};
		if (vkEnumerateInstanceLayerProperties(&supportedLayerCount, nullptr) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't query instance's validation layer's count. As a consequency, all layers will be disabled");
			return false;
		}

		layerProperties.resize(supportedLayerCount);
		if (vkEnumerateInstanceLayerProperties(&supportedLayerCount, layerProperties.data()) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't query instance's validation layer. As a consequency, all layers will be disabled");
			return false;
		}

		return checkValidationLayerValidity(layerName);
	}

#endif



	auto checkExtensionValidity(const char *extensionName) noexcept -> bool {
		static std::vector<VkExtensionProperties> extensionProperties {};
		static bool initialized {false};

		if (initialized) {
			bool supported {std::ranges::find_if(extensionProperties, [&extensionName](const VkExtensionProperties &properties) -> bool {
				return strcmp(properties.extensionName, extensionName) == 0;
			}) != extensionProperties.end()};
			
			if (!supported)
				sl::mainLogger.warn("Instance extension '{}' is not supported. It will not be enabled", extensionName);

			return supported;
		}

		initialized = true;
		std::uint32_t supportedExtensionCount {};
		if (vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, nullptr) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't query instance's extension's count. As a consequency, all extensions will be disabled");
			return false;
		}

		extensionProperties.resize(supportedExtensionCount);
		if (vkEnumerateInstanceExtensionProperties(nullptr, &supportedExtensionCount, extensionProperties.data()) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't query instance's extension. As a consequency, all extensions will be disabled");
			return false;
		}

		return checkExtensionValidity(extensionName);
	}


	VKAPI_ATTR auto VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT severity,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
		void*
	) -> VkBool32 {
		sl::utils::LogSeverity logSeverity {};

		switch (severity) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: logSeverity = sl::utils::LogSeverity::eDebug; break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    logSeverity = sl::utils::LogSeverity::eInfo;  break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: logSeverity = sl::utils::LogSeverity::eWarn;  break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   logSeverity = sl::utils::LogSeverity::eError; break;
			default: logSeverity = sl::utils::LogSeverity::eDebug; break;
		}

		sl::mainLogger.log(logSeverity, "Vulkan validation layer : {}", callbackData->pMessage);
		return VK_FALSE;
	}


	auto Instance::create(const InstanceCreateInfos &createInfos) noexcept -> sl::Result {
		m_window = createInfos.window;

		std::vector<const char*> requiredExtensions {};
		std::uint32_t sdlRequiredExtensionsCount {};
		const char *const *sdlRequiredExtensions {SDL_Vulkan_GetInstanceExtensions(&sdlRequiredExtensionsCount)};
		(void)requiredExtensions.insert(requiredExtensions.end(), sdlRequiredExtensions, sdlRequiredExtensions + sdlRequiredExtensionsCount);
		bool isAllRequiredExtensionsSupported {std::ranges::fold_left(std::views::transform(requiredExtensions, checkExtensionValidity), true, std::logical_and<bool> ())};
		if (!isAllRequiredExtensionsSupported)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Required vulkan instance's extensions are not supported");

		std::vector<const char*> extensions {
		#ifndef NDEBUG
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME
		#endif
		};
		extensions = std::ranges::to<std::vector<const char*>> (extensions | std::views::filter(checkExtensionValidity));

		(void)extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

	#ifndef NDEBUG
		std::vector<const char*> validationLayers {"VK_LAYER_KHRONOS_validation"};
		validationLayers = std::ranges::to<std::vector<const char*>> (validationLayers | std::views::filter(checkValidationLayerValidity));
	#endif

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
		instanceCreateInfos.enabledExtensionCount = extensions.size();
		instanceCreateInfos.ppEnabledExtensionNames = extensions.data();

	#ifndef NDEBUG
		instanceCreateInfos.enabledLayerCount = validationLayers.size();
		instanceCreateInfos.ppEnabledLayerNames = validationLayers.data();
	#else
		instanceCreateInfos.enabledLayerCount = 0;
		instanceCreateInfos.ppEnabledLayerNames = nullptr;
	#endif

		if (vkCreateInstance(&instanceCreateInfos, nullptr, &m_instance) != VK_SUCCESS)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create vulkan instance");

		m_getInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr> (SDL_Vulkan_GetVkGetInstanceProcAddr());
		if (m_getInstanceProcAddr == nullptr)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't load vkGetInstanceProcAddr");

	#ifndef NDEBUG
		if (std::ranges::find_if(extensions, [](const char *extensionName) {return strcmp(extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0;}) == extensions.end())
			return sl::Result::eSuccess;

		auto createDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT> (m_getInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT"));
		if (createDebugUtilsMessengerEXT == nullptr) {
			sl::mainLogger.warn("Can't load vkCreateDebugUtilsMessengerEXT");
			return sl::Result::eSuccess;
		}

		VkDebugUtilsMessengerCreateInfoEXT debugUtilsMessengerCreateInfos {};
		debugUtilsMessengerCreateInfos.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugUtilsMessengerCreateInfos.pUserData = nullptr;
		debugUtilsMessengerCreateInfos.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugUtilsMessengerCreateInfos.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugUtilsMessengerCreateInfos.pfnUserCallback = debugCallback;
		if (createDebugUtilsMessengerEXT(m_instance, &debugUtilsMessengerCreateInfos, nullptr, &m_debugUtilsMessenger) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't create debug vulkan utils messenger");
			return sl::Result::eSuccess;
		}
	#endif

		(void)vkEnumeratePhysicalDevices(m_instance, nullptr, nullptr);

		return sl::Result::eSuccess;
	}


	auto Instance::destroy() noexcept -> void {
	#ifndef NDEBUG
		if (m_getInstanceProcAddr != nullptr && m_debugUtilsMessenger != VK_NULL_HANDLE) {
			auto destroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT> (m_getInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT"));
			if (destroyDebugUtilsMessengerEXT == nullptr)
				sl::mainLogger.warn("Can't load vkDestroyDebugUtilsMessengerEXT");
			else
				destroyDebugUtilsMessengerEXT(m_instance, m_debugUtilsMessenger, nullptr);
		}
	#endif

		if (m_instance != VK_NULL_HANDLE)
			vkDestroyInstance(m_instance, nullptr);
	}

} // namespace sl::render::vulkan
