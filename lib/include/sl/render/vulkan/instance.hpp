#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/utils/string.hpp"
#include "sl/utils/utils.hpp"
#include "sl/result.hpp"
#include "sl/window.hpp"

#include "sl/render/vulkan/GPU.hpp"


namespace sl::render::vulkan {
	struct InstanceCreateInfos {
		sl::String appName;
		sl::utils::Version appVersion;
		sl::Window *window;
	};

	class SL_CORE Instance final {
		public:
			Instance() noexcept = default;
			~Instance() = default;

			auto create(const InstanceCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getInstance() const noexcept -> VkInstance {return m_instance;}
			inline auto getVkGetInstanceProcAddr() const noexcept -> PFN_vkGetInstanceProcAddr {return m_getInstanceProcAddr;}
			inline auto getSurface() const noexcept -> VkSurfaceKHR {return m_surface;}


		private:
			VkInstance m_instance;
			PFN_vkGetInstanceProcAddr m_getInstanceProcAddr;
		#ifndef NDEBUG
			VkDebugUtilsMessengerEXT m_debugUtilsMessenger;
		#endif
			sl::Window *m_window;
			VkSurfaceKHR m_surface;
			sl::render::vulkan::GPU m_gpu;
	};

} // namespace sl::render::vulkan
