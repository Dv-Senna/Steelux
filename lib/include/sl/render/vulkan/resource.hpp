#pragma once

#include <optional>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/memory/gpu/allocator.hpp"
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

	template <sl::memory::gpu::IsAllocator Allocator>
	struct ResourceCreateInfos {
		sl::render::vulkan::Instance *instance;
		Allocator allocator;
		ResourceType type;
		sl::utils::Bytes size;
		bool graphicsUsable;
		bool computeUsable;
	};


	template <sl::memory::gpu::IsAllocator Allocator>
	class SL_CORE Resource {
		public:
			Resource() noexcept = default;
			~Resource() = default;

			auto create(const ResourceCreateInfos<Allocator> &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			inline auto getType() const noexcept -> ResourceType {return m_type;}
			auto getBuffer() const noexcept -> std::optional<VkBuffer>;

			template <std::ranges::input_range Range>
			auto set(Range &&value) noexcept -> sl::Result;


		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			Allocator m_allocator;
			sl::utils::Bytes m_size;
			ResourceType m_type;
			std::size_t m_memory;
			union {
				VkBuffer m_buffer;
			};
	};

} // namespace sl::render::vulkan

#include "sl/render/vulkan/resource.inl"
