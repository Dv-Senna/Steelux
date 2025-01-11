#pragma once

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

		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			VkSwapchainKHR m_swapchain;
	};

} // namespace sl::render::vulkan
