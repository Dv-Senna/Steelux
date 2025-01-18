#pragma once

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/memory/gpu/allocator.hpp"
#include "sl/utils/units.hpp"


namespace sl::render::vulkan {
	struct Instance;
	struct GPU;
} // namespace sl::render::vulkan

namespace sl::memory::gpu {
	using namespace sl::utils::literals;

	struct HeapAllocatorCreateInfos {
		sl::render::vulkan::Instance *instance;
		bool cpuVisible {false};
		sl::utils::Bytes size {512_kiB};
		sl::utils::Bytes averageAllocationSize {512_B};
	};

	class SL_CORE HeapAllocator {
		public:
			HeapAllocator() noexcept = default;
			~HeapAllocator() = default;

			auto create(const HeapAllocatorCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			auto allocate(sl::utils::Bytes size, sl::utils::Bytes alignment) noexcept -> std::optional<std::size_t>;
			auto deallocate(std::size_t position, sl::utils::Bytes size) noexcept -> void;

			inline auto getMemory() const noexcept -> VkDeviceMemory {return m_memory;}

		private:
			struct Allocation {
				std::size_t position;
				sl::utils::Bytes size;
			};

			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			bool m_cpuVisible;
			sl::utils::Bytes m_size;
			std::vector<Allocation> m_allocations;
			VkDeviceMemory m_memory;
	};


	class HeapAllocatorView {
		public:
			using GpuAllocatorTag = void;

			HeapAllocatorView() noexcept = default;
			inline HeapAllocatorView(HeapAllocator &allocator) noexcept : m_allocator {&allocator} {}
			~HeapAllocatorView() = default;

			inline auto allocate(sl::utils::Bytes size, sl::utils::Bytes alignment) noexcept -> std::optional<std::size_t> {
				return m_allocator->allocate(size, alignment);
			}
			inline auto deallocate(std::size_t position, sl::utils::Bytes size) noexcept -> void {
				m_allocator->deallocate(position, size);
			}
			inline auto getMemory() const noexcept -> VkDeviceMemory {return m_allocator->getMemory();}

		private:
			HeapAllocator *m_allocator;
	};


	static_assert(sl::memory::gpu::IsAllocator<HeapAllocatorView>);

} // namespace sl::memory::gpu
