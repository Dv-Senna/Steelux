#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"


namespace sl::render::vulkan {
	class Instance;

	struct GPUCreateInfos {
		Instance *instance;
	};

	class SL_CORE GPU {
		public:
			GPU() noexcept = default;
			~GPU() = default;

			auto create(const GPUCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getDevice() const noexcept -> VkDevice {return m_device;}

		private:
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;
			Instance *m_instance;
	};

} // namespace sl::render::vulkan
