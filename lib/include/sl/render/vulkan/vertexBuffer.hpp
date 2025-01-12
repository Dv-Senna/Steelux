#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"


namespace sl::render::vulkan {
	class Instance;
	class GPU;

	struct VertexBufferCreateInfos {
		sl::render::vulkan::Instance *instance;
		std::size_t vertexComponentCount;
		std::vector<float> vertices;
	};

	class SL_CORE VertexBuffer {
		public:
			VertexBuffer() noexcept = default;
			~VertexBuffer() = default;

			auto create(const VertexBufferCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getBuffer() const noexcept -> VkBuffer {return m_buffer;}


		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			VkBuffer m_buffer;
			VkDeviceMemory m_memory;
			std::size_t m_verticesCount;
			std::size_t m_vertexComponentCount;
	};

} // namespace sl::render::vulkan
