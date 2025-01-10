#include "sl/render/vulkan/GPU.hpp"

#include <optional>
#include <ranges>
#include <vector>

#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/errorStack.hpp"
#include "sl/utils/logger.hpp"


namespace sl::render::vulkan {
	auto scorePhysicalDevice(
		VkPhysicalDevice physicalDevice,
		const std::vector<const char*> &extensions,
		const std::vector<const char*> &requiredExtensions,
		const VkPhysicalDeviceFeatures &features,
		const VkPhysicalDeviceFeatures &requiredFeatures
	) -> std::optional<std::uint32_t> {
		VkPhysicalDeviceFeatures deviceFeatures {};
		vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);

		VkPhysicalDeviceProperties deviceProperties {};
		vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

		std::uint32_t deviceExtensionsCount {};
		std::vector<VkExtensionProperties> deviceExtensions {};
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, nullptr) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't get physical device '{}' extensions count", deviceProperties.deviceName);
			return std::nullopt;
		}

		deviceExtensions.resize(deviceExtensionsCount);
		if (vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionsCount, deviceExtensions.data()) != VK_SUCCESS) {
			sl::mainLogger.warn("Can't get physical device '{}' extensions", deviceProperties.deviceName);
			return std::nullopt;
		}

		std::uint32_t score {};
		if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 1000;

		for (std::size_t i {0}; i < sizeof(deviceFeatures) / sizeof(VkBool32); ++i) {
			if (reinterpret_cast<const VkBool32*> (&features)[i] && reinterpret_cast<VkBool32*> (&deviceFeatures)[i])
				score += 10;
			if (reinterpret_cast<const VkBool32*> (&requiredFeatures)[i] && !reinterpret_cast<VkBool32*> (&deviceFeatures)[i])
				return std::nullopt;
		}

		score += std::ranges::fold_left(extensions | std::views::transform([&](const char *extensionName) -> std::uint32_t {
			return std::ranges::find_if(deviceExtensions, [&extensionName] (
				const VkExtensionProperties &extension) -> bool {return strcmp(extensionName, extension.extensionName) == 0;}
			) != deviceExtensions.end() ? 10 : 0;
		}), std::uint32_t(0), std::plus<std::uint32_t> ());

		bool isRequiredExtensionsMissing {std::ranges::fold_left(requiredExtensions | std::views::transform([&](const char *extensionName) -> bool {
			return std::ranges::find_if(deviceExtensions, [&extensionName] (
						const VkExtensionProperties &extension) -> bool {return strcmp(extensionName, extension.extensionName) == 0;}
			) == deviceExtensions.end();
		}), false, std::logical_or<bool> ())};

		if (isRequiredExtensionsMissing)
			return std::nullopt;
		return score;
	}

	auto selectBestPhysicalDevice(
		VkInstance instance,
		const std::vector<const char*> &extensions,
		const std::vector<const char*> &requiredExtensions,
		const VkPhysicalDeviceFeatures &features,
		const VkPhysicalDeviceFeatures &requiredFeatures
	) -> VkPhysicalDevice {
		std::vector<VkPhysicalDevice> physicalDevices {};
		std::uint32_t physicalDeviceCount {};
		if (vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr) != VK_SUCCESS)
			return sl::utils::ErrorStack::push(VK_NULL_HANDLE, "Can't get physical devices count");

		physicalDevices.resize(physicalDeviceCount);
		if (vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS)
			return sl::utils::ErrorStack::push(VK_NULL_HANDLE, "Can't get physical devices");

		std::vector<std::pair<std::uint32_t, VkPhysicalDevice>> scoredPhysicalDevices {std::ranges::to<decltype(scoredPhysicalDevices)> (
			physicalDevices | std::views::transform([&] (VkPhysicalDevice physicalDevice) {return std::pair<std::optional<std::uint32_t>, VkPhysicalDevice> (scorePhysicalDevice(
				physicalDevice,
				extensions,
				requiredExtensions,
				features,
				requiredFeatures
			), physicalDevice);}) | std::views::filter([](const auto &scoredDevice) -> bool {return !!scoredDevice.first;})
			| std::views::transform([](const auto &scoredDevice) -> std::pair<std::uint32_t, VkPhysicalDevice> {return {*scoredDevice.first, scoredDevice.second};})
		)};

		if (scoredPhysicalDevices.empty())
			return VK_NULL_HANDLE;

		std::ranges::sort(scoredPhysicalDevices, [](const auto &lhs, const auto &rhs) {return lhs.first < rhs.first;});
		return scoredPhysicalDevices.back().second;
	};


	struct QueueFamilyIndices {
		std::uint32_t graphics;
		std::uint32_t compute;
		std::uint32_t transfer;
		std::uint32_t present;
	};

	auto selectQueueFamilies(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface,
		const std::vector<VkQueueFamilyProperties> &queueFamilyProperties
	) noexcept -> QueueFamilyIndices {
		QueueFamilyIndices indices {};

		enum class QueueCapability {
			eGraphics,
			eCompute,
			eTransfer,
			ePresent
		};

		return indices;
	}


	auto GPU::create(const GPUCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;

		std::vector<const char*> extensions {};
		std::vector<const char*> requiredExtensions {};
		VkPhysicalDeviceFeatures features {};
		VkPhysicalDeviceFeatures requiredFeatures {};

		m_physicalDevice = selectBestPhysicalDevice(m_instance->getInstance(), extensions, requiredExtensions, features, requiredFeatures);
		if (m_physicalDevice == VK_NULL_HANDLE)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't find appropriate GPU");

		extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());
		for (std::size_t i {0}; i < sizeof(features) / sizeof(VkBool32); ++i) {
			if (reinterpret_cast<VkBool32*> (&requiredFeatures)[i])
				reinterpret_cast<VkBool32*> (&features)[i] = VK_TRUE;
		}

		VkPhysicalDeviceProperties physicalDeviceProperties {};
		vkGetPhysicalDeviceProperties(m_physicalDevice, &physicalDeviceProperties);

		sl::mainLogger.info("Chosen GPU : '{}'", physicalDeviceProperties.deviceName);

		std::uint32_t queueFamilyPropertiesCount {};
		std::vector<VkQueueFamilyProperties> queueFamilyProperties {};
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertiesCount, nullptr);
		queueFamilyProperties.resize(queueFamilyPropertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyPropertiesCount, queueFamilyProperties.data());

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos {};
		deviceQueueCreateInfos.reserve(queueFamilyPropertiesCount);
		float priorities {1.f};

		for (std::size_t i : std::views::iota(std::size_t(0), queueFamilyProperties.size())) {
			VkDeviceQueueCreateInfo queueCreateInfos {};
			queueCreateInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfos.queueFamilyIndex = static_cast<std::uint32_t> (i);
			queueCreateInfos.queueCount = queueFamilyProperties[i].queueCount;
			queueCreateInfos.pQueuePriorities = &priorities;
			deviceQueueCreateInfos.push_back(queueCreateInfos);
		}

		VkDeviceCreateInfo deviceCreateInfos {};
		deviceCreateInfos.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfos.enabledLayerCount = 0;
		deviceCreateInfos.ppEnabledLayerNames = nullptr;
		deviceCreateInfos.enabledExtensionCount = extensions.size();
		deviceCreateInfos.ppEnabledExtensionNames = extensions.data();
		deviceCreateInfos.pEnabledFeatures = &features;
		deviceCreateInfos.queueCreateInfoCount = deviceQueueCreateInfos.size();
		deviceCreateInfos.pQueueCreateInfos = deviceQueueCreateInfos.data();
		if (vkCreateDevice(m_physicalDevice, &deviceCreateInfos, nullptr, &m_device) != VK_SUCCESS)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create logical device");


		VkDeviceQueueInfo2 queueInfos {};
		queueInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
		queueInfos.queueFamilyIndex = 0;
		queueInfos.queueIndex = 0;
		VkQueue queue {};
		vkGetDeviceQueue2(m_device, &queueInfos, &queue);

		return sl::Result::eSuccess;
	}


	auto GPU::destroy() noexcept -> void {
		if (m_device != VK_NULL_HANDLE)
			vkDestroyDevice(m_device, nullptr);
	}

} // namespace sl::render::vulkan
