#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/enums.hpp"


namespace sl::render::vulkan {
	using namespace sl::utils::literals;

	class Instance;

	struct GPUCreateInfos {
		Instance *instance;
	};

	class SL_CORE GPU {
		public:
			enum class QueueCapability : sl::utils::PackedEnumString {
				eGraphics = "graphics"_pes,
				eCompute = "compute"_pes,
				eTransfer = "transfer"_pes,
				ePresent = "present"_pes
			};

			struct Queue {
				std::uint32_t familyIndex;
				std::vector<VkQueue> queues;
			};

			GPU() noexcept = default;
			~GPU() = default;

			auto create(const GPUCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getPhysicalDevice() const noexcept -> VkPhysicalDevice {return m_physicalDevice;}
			inline auto getDevice() const noexcept -> VkDevice {return m_device;}
			inline auto getGraphicsQueue() const noexcept -> const Queue& {return m_queues.find(QueueCapability::eGraphics)->second;}
			inline auto getComputeQueue() const noexcept -> const Queue& {return m_queues.find(QueueCapability::eCompute)->second;}
			inline auto getTransferQueue() const noexcept -> const Queue& {return m_queues.find(QueueCapability::eTransfer)->second;}
			inline auto getPresentQueue() const noexcept -> const Queue& {return m_queues.find(QueueCapability::ePresent)->second;}

		private:
			VkPhysicalDevice m_physicalDevice;
			VkDevice m_device;
			Instance *m_instance;
			std::map<QueueCapability, Queue> m_queues;
	};

} // namespace sl::render::vulkan
