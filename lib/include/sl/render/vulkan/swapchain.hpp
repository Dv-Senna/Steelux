#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"


namespace sl::render::vulkan {
	class Instance;
	class GPU;

	struct SwapchainCreateInfos {
		sl::render::vulkan::Instance *instance;
	};

	class SL_CORE Swapchain {
		public:
			Swapchain() noexcept = default;
			~Swapchain() = default;

			auto create(const sl::render::vulkan::SwapchainCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getSwapchain() const noexcept -> VkSwapchainKHR {return m_swapchain;}
			inline auto getImages() const noexcept -> const std::vector<VkImage>& {return m_images;}
			inline auto getImageViews() const noexcept -> const std::vector<VkImageView>& {return m_imageViews;}
			inline auto getImageExtent() const noexcept -> const VkExtent2D& {return m_imageExtent;}


		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			VkSwapchainKHR m_swapchain;
			VkSwapchainCreateInfoKHR m_swapchainCreateInfos;
			std::vector<VkImage> m_images;
			std::vector<VkImageView> m_imageViews;
			VkExtent2D m_imageExtent;
	};

} // namespace sl::render::vulkan
