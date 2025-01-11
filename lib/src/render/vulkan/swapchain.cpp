#include "sl/render/vulkan/swapchain.hpp"

#include <optional>
#include <set>
#include <vector>

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/render/vulkan/utils.hpp"
#include "sl/utils/errorStack.hpp"
#include "sl/utils/logger.hpp"


namespace sl::render::vulkan {
	auto generateSwapchainCreateInfos(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface,
		std::uint32_t graphicsQueueFamilyIndex,
		std::uint32_t presentQueueFamilyIndex
	) -> std::optional<VkSwapchainCreateInfoKHR> {

		VkSurfaceCapabilitiesKHR surfaceCapabilities {};
		if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities) != VK_SUCCESS)
			return sl::ErrorStack::push(std::nullopt, "Can't retrieve surface capabilities for swapchain create infos");


		auto surfaceFormats {sl::render::vulkan::getVulkanMultivalue<VkSurfaceFormatKHR> (vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, surface)};
		if (!surfaceFormats)
			return sl::ErrorStack::push(std::nullopt, "Can't get supported surface formats for swapchain create infos");

		auto surfaceFormat {std::ranges::find_if(*surfaceFormats, [](const VkSurfaceFormatKHR &surfaceFormat) -> bool {
			return surfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB && surfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		})};
		if (surfaceFormat == surfaceFormats->end())
			return sl::ErrorStack::push(std::nullopt, "Can't find valid surface format for swapchain create infos");


		auto surfacePresentModes {sl::render::vulkan::getVulkanMultivalue<VkPresentModeKHR> (vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice, surface)};
		if (!surfacePresentModes)
			return sl::ErrorStack::push(std::nullopt, "Can't get supported present modes for swapchain create infos");

		auto presentModeIt {std::ranges::find_if(*surfacePresentModes, [](const VkPresentModeKHR &presentMode) -> bool {
			return presentMode == VK_PRESENT_MODE_MAILBOX_KHR;
		})};
		VkPresentModeKHR presentMode {};
		if (presentModeIt != surfacePresentModes->end())
			presentMode = *presentModeIt;
		else
			presentMode = VK_PRESENT_MODE_FIFO_KHR;


		std::set<std::uint32_t> queueFamilyIndicesSet {graphicsQueueFamilyIndex, presentQueueFamilyIndex};
		static std::vector<std::uint32_t> queueFamilyIndices {queueFamilyIndicesSet.begin(), queueFamilyIndicesSet.end()};
		sl::mainLogger.info("Swapchain extent : {}, {}", surfaceCapabilities.currentExtent.width, surfaceCapabilities.currentExtent.height);

		VkSwapchainCreateInfoKHR swapchainCreateInfos {};
		swapchainCreateInfos.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfos.surface = surface;
		swapchainCreateInfos.minImageCount = surfaceCapabilities.minImageCount;
		swapchainCreateInfos.imageFormat = surfaceFormat->format;
		swapchainCreateInfos.imageColorSpace = surfaceFormat->colorSpace;
		swapchainCreateInfos.imageExtent = surfaceCapabilities.currentExtent;
		swapchainCreateInfos.imageArrayLayers = 1;
		swapchainCreateInfos.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfos.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfos.queueFamilyIndexCount = queueFamilyIndices.size();
		swapchainCreateInfos.pQueueFamilyIndices = queueFamilyIndices.data();
		swapchainCreateInfos.preTransform = surfaceCapabilities.currentTransform;
		swapchainCreateInfos.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfos.presentMode = presentMode;
		swapchainCreateInfos.clipped = VK_TRUE;
		swapchainCreateInfos.oldSwapchain = VK_NULL_HANDLE;

		return swapchainCreateInfos;
	}


	auto Swapchain::create(const sl::render::vulkan::SwapchainCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();

		std::optional<VkSwapchainCreateInfoKHR> swapchainCreateInfos {generateSwapchainCreateInfos(
			m_gpu->getPhysicalDevice(),
			m_instance->getSurface(),
			m_gpu->getGraphicsQueue().familyIndex,
			m_gpu->getPresentQueue().familyIndex 
		)};
		if (!swapchainCreateInfos)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't generate swapchain create infos");
		m_swapchainCreateInfos = *swapchainCreateInfos;

		if (vkCreateSwapchainKHR(m_gpu->getDevice(), &m_swapchainCreateInfos, nullptr, &m_swapchain) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create swapchain");

		auto swapchainImages {sl::render::vulkan::getVulkanMultivalue<VkImage> (vkGetSwapchainImagesKHR, m_gpu->getDevice(), m_swapchain)};
		if (!swapchainImages)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't get swapchain's images");
		m_images = *swapchainImages;

		m_imageViews.reserve(m_images.size());
		VkImageViewCreateInfo imageViewCreateInfos {};
		imageViewCreateInfos.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfos.format = m_swapchainCreateInfos.imageFormat;
		imageViewCreateInfos.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfos.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfos.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfos.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfos.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfos.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfos.subresourceRange.levelCount = 1;
		imageViewCreateInfos.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfos.subresourceRange.layerCount = 1;
		imageViewCreateInfos.subresourceRange.baseArrayLayer = 0;

		for (const auto &image : m_images) {
			imageViewCreateInfos.image = image;
			VkImageView imageView {};
			if (vkCreateImageView(m_gpu->getDevice(), &imageViewCreateInfos, nullptr, &imageView) != VK_SUCCESS)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't create swapchain's image view");
			m_imageViews.push_back(imageView);
		}

		return sl::Result::eSuccess;
	}


	auto Swapchain::destroy() noexcept -> void {
		for (const auto &imageView : m_imageViews)
			vkDestroyImageView(m_gpu->getDevice(), imageView, nullptr);
		if (m_swapchain != VK_NULL_HANDLE) {
			vkDestroySwapchainKHR(m_gpu->getDevice(), m_swapchain, nullptr);
		}
	}

} // namespace sl::render::vulkan
