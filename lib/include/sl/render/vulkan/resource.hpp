#pragma once

#include <optional>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/enums.hpp"
#include "sl/utils/units.hpp"


namespace sl::render::vulkan {
	using namespace sl::utils::literals;

	class Instance;
	class GPU;

	enum class ResourceType : sl::utils::PackedEnumString {
		eVertexBuffer = "vertex"_pes,
		eStagingBuffer = "staging"_pes,
		eUniformBuffer = "uniform"_pes,
		eIndexBuffer = "index"_pes,
		eStorageBuffer = "storage"_pes
	};

	struct ResourceCreateInfos {
		sl::render::vulkan::Instance *instance;
		ResourceType type;
		sl::utils::Bytes size;
		bool graphicsUsable;
		bool computeUsable;
	};


	class SL_CORE Resource {
		public:
			Resource() noexcept = default;
			~Resource() = default;

			auto create(const ResourceCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getType() const noexcept -> ResourceType {return m_type;}
			auto getBuffer() const noexcept -> std::optional<VkBuffer>;


		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			sl::utils::Bytes m_size;
			ResourceType m_type;
			union {
				VkBuffer m_buffer;
			};
	};

} // namespace sl::render::vulkan
