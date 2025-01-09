#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/utils/string.hpp"
#include "sl/utils/utils.hpp"
#include "sl/result.hpp"
#include "sl/window.hpp"


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

			inline auto getInstance() noexcept -> VkInstance {return m_instance;}


		private:
			VkInstance m_instance;
			PFN_vkGetInstanceProcAddr m_getInstanceProcAddr;
		#ifndef NDEBUG
			VkDebugUtilsMessengerEXT m_debugUtilsMessenger;
		#endif
			sl::Window *m_window;
	};

} // namespace sl::render::vulkan
