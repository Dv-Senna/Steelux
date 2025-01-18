#include "sl/memory/gpu/heapAllocator.hpp"

#include <algorithm>

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/errorStack.hpp"


namespace sl::memory::gpu {
	auto HeapAllocator::create(const HeapAllocatorCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_cpuVisible = createInfos.cpuVisible;
		m_size = createInfos.size;
		m_allocations.reserve(m_size / createInfos.averageAllocationSize);

		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties {};
		vkGetPhysicalDeviceMemoryProperties(m_gpu->getPhysicalDevice(), &physicalDeviceMemoryProperties);

		VkMemoryPropertyFlags memoryPropertyFlags {};
		if (m_cpuVisible)
			memoryPropertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		else
			memoryPropertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

		std::uint32_t memoryTypeIndex {static_cast<std::uint32_t> (std::find_if(
			physicalDeviceMemoryProperties.memoryTypes,
			physicalDeviceMemoryProperties.memoryTypes + physicalDeviceMemoryProperties.memoryTypeCount,
			[memoryPropertyFlags](const VkMemoryType &type){
				return type.propertyFlags & memoryPropertyFlags;
			}
		) - physicalDeviceMemoryProperties.memoryTypes)};

		VkMemoryAllocateInfo allocateInfos {};
		allocateInfos.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocateInfos.memoryTypeIndex = memoryTypeIndex;
		allocateInfos.allocationSize = m_size;
		if (vkAllocateMemory(m_gpu->getDevice(), &allocateInfos, nullptr, &m_memory) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't allocate memory for gpu heap allocator");

		return sl::Result::eSuccess;
	}


	auto HeapAllocator::destroy() noexcept -> void {
		if (m_memory != VK_NULL_HANDLE)
			vkFreeMemory(m_gpu->getDevice(), m_memory, nullptr);
	}


	auto HeapAllocator::allocate(sl::utils::Bytes size, sl::utils::Bytes alignment) noexcept -> std::optional<std::size_t> {
		std::size_t lastEndPosition {0};
		auto allocation {m_allocations.begin()};

		for (; allocation != m_allocations.end(); ++allocation) {
			if (lastEndPosition % alignment != 0)
				lastEndPosition += alignment - (lastEndPosition % alignment);

			if (size < allocation->position - lastEndPosition)
				break;
			lastEndPosition = allocation->position + allocation->size;
		}

		if (m_size - lastEndPosition < size)
			return std::nullopt;

		allocation = m_allocations.insert(allocation, {lastEndPosition, size});
		return allocation->position + 1;
	}


	auto HeapAllocator::deallocate(std::size_t position, sl::utils::Bytes) noexcept -> void {
		--position;
		for (auto it {m_allocations.begin()}; it != m_allocations.end() && it->position <= position; ++it) {
			if (it->position != position)
				continue;
			m_allocations.erase(it);
			return;
		}
	}

} // namespace sl::memory::gpu
