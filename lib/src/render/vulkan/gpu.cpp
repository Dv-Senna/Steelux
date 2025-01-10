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


	auto selectQueueFamilies(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface,
		const std::vector<VkQueueFamilyProperties> &queueFamilyProperties
	) noexcept -> std::map<GPU::QueueCapability, std::uint32_t> {
		std::map<GPU::QueueCapability, std::uint32_t> indices {
			{GPU::QueueCapability::eGraphics, std::numeric_limits<std::uint32_t>::max()},
			{GPU::QueueCapability::eCompute, std::numeric_limits<std::uint32_t>::max()},
			{GPU::QueueCapability::eTransfer, std::numeric_limits<std::uint32_t>::max()},
			{GPU::QueueCapability::ePresent, std::numeric_limits<std::uint32_t>::max()}
		};


		std::vector<std::vector<GPU::QueueCapability>> queueCapabilities {};
		queueCapabilities.reserve(queueFamilyProperties.size());

		for (std::size_t familyIndex : std::views::iota(std::size_t(0), queueFamilyProperties.size())) {
			std::vector<GPU::QueueCapability> capabilities {};
			if (queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				capabilities.push_back(GPU::QueueCapability::eGraphics);
			if (queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_COMPUTE_BIT)
				capabilities.push_back(GPU::QueueCapability::eCompute);
			if (queueFamilyProperties[familyIndex].queueFlags & VK_QUEUE_TRANSFER_BIT)
				capabilities.push_back(GPU::QueueCapability::eTransfer);

			VkBool32 presentSupport {VK_FALSE};
			if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, familyIndex, surface, &presentSupport) != VK_SUCCESS)
				presentSupport = VK_FALSE;
			if (presentSupport)
				capabilities.push_back(GPU::QueueCapability::ePresent);

			queueCapabilities.push_back(capabilities);

			if (capabilities.size() == 1 && indices[capabilities[0]] == std::numeric_limits<std::uint32_t>::max()) {
				indices[capabilities[0]] = familyIndex;
				continue;
			}
		}

		for (auto &index : indices) {
			if (index.second != std::numeric_limits<std::uint32_t>::max())
				continue;

			for (std::size_t familyIndex : std::views::iota(std::size_t(0), queueCapabilities.size())) {
				if (std::ranges::find(queueCapabilities[familyIndex], index.first) == queueCapabilities[familyIndex].end())
					continue;
				index.second = familyIndex;
				break;
			}
		}

		return indices;
	}


	auto getQueueCreateInfos(
		const std::map<GPU::QueueCapability, std::uint32_t> &indices,
		const std::vector<VkQueueFamilyProperties> &queueProperties
	) noexcept -> std::vector<VkDeviceQueueCreateInfo> {
		static float priority {1.f};

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos {};
		queueCreateInfos.reserve(sizeof(indices) / sizeof(std::uint32_t));

		for (const auto index : indices) {
			if (std::ranges::find_if(queueCreateInfos, [&index](const VkDeviceQueueCreateInfo &createInfos) -> bool {
				return createInfos.queueFamilyIndex == index.second;
			}) != queueCreateInfos.end())
				continue;

			VkDeviceQueueCreateInfo createInfos {};
			createInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			createInfos.queueFamilyIndex = index.second;
			createInfos.queueCount = queueProperties[index.second].queueCount;
			createInfos.pQueuePriorities = &priority;
			queueCreateInfos.push_back(createInfos);
		}

		return queueCreateInfos;
	};


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

		std::map<GPU::QueueCapability, std::uint32_t> queueIndices {selectQueueFamilies(m_physicalDevice, m_instance->getSurface(), queueFamilyProperties)};

		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos {getQueueCreateInfos(queueIndices, queueFamilyProperties)};

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


		std::map<std::uint32_t, std::vector<VkQueue>> queuesPerFamilies {};
		for (const auto &queueCreateInfos : deviceQueueCreateInfos) {
			std::vector<VkQueue> queues {};
			queues.reserve(queueCreateInfos.queueCount);
			VkDeviceQueueInfo2 queueInfos {};
			queueInfos.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
			queueInfos.queueFamilyIndex = queueCreateInfos.queueFamilyIndex;

			for (std::uint32_t i : std::views::iota(std::uint32_t(0), queueCreateInfos.queueCount)) {
				queueInfos.queueIndex = i;
				VkQueue queue {};
				vkGetDeviceQueue2(m_device, &queueInfos, &queue);
				queues.push_back(queue);
			}

			queuesPerFamilies[queueCreateInfos.queueFamilyIndex] = queues;
		}


		for (const auto &index : queueIndices) {
			m_queues[index.first].familyIndex = index.second;
			m_queues[index.first].queues = queuesPerFamilies[index.second];
		}

		sl::mainLogger.info("The following queue families were created with the logical device :");
		for (const auto &index : m_queues)
			sl::mainLogger.info("\t- {} : family={}, count={}", index.first, index.second.familyIndex, index.second.queues.size());

		return sl::Result::eSuccess;
	}


	auto GPU::destroy() noexcept -> void {
		if (m_device != VK_NULL_HANDLE)
			vkDestroyDevice(m_device, nullptr);
	}

} // namespace sl::render::vulkan
