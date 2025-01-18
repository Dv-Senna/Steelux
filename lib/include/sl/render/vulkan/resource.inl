#pragma once

#include "sl/render/vulkan/resource.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/errorStack.hpp"
#include "sl/utils/utils.hpp"


namespace sl::render::vulkan {
	auto isResourceBuffer(ResourceType type) noexcept -> bool {
		static const std::vector<ResourceType> buffers {
			ResourceType::eIndexBuffer,
			ResourceType::eStagingBuffer,
			ResourceType::eStorageBuffer,
			ResourceType::eUniformBuffer,
			ResourceType::eVertexBuffer
		};
		return std::ranges::find(buffers, type) != buffers.end();
	};


	template <sl::memory::gpu::IsAllocator Allocator>
	auto Resource<Allocator>::create(const ResourceCreateInfos<Allocator> &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_type = createInfos.type;
		m_size = createInfos.size;
		m_allocator = createInfos.allocator;

		if (!isResourceBuffer(m_type))
			return sl::Result::eFailure;

		static const std::unordered_map<ResourceType, VkBufferUsageFlags> bufferUsages {
			{ResourceType::eStagingBuffer, VK_BUFFER_USAGE_TRANSFER_SRC_BIT},
			{ResourceType::eIndexBuffer,   VK_BUFFER_USAGE_INDEX_BUFFER_BIT   | VK_BUFFER_USAGE_TRANSFER_DST_BIT},
			{ResourceType::eStorageBuffer, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT},
			{ResourceType::eUniformBuffer, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT},
			{ResourceType::eVertexBuffer,  VK_BUFFER_USAGE_VERTEX_BUFFER_BIT  | VK_BUFFER_USAGE_TRANSFER_DST_BIT}
		};


		std::unordered_set<std::uint32_t> queueFamilyIndicesSet {m_gpu->getTransferQueue().familyIndex};
		if (m_type != ResourceType::eStagingBuffer && createInfos.graphicsUsable)
			queueFamilyIndicesSet.insert(m_gpu->getGraphicsQueue().familyIndex);
		if (m_type != ResourceType::eStagingBuffer && createInfos.computeUsable)
			queueFamilyIndicesSet.insert(m_gpu->getComputeQueue().familyIndex);
		std::vector<std::uint32_t> queueFamilyIndices {queueFamilyIndicesSet.begin(), queueFamilyIndicesSet.end()};

		VkBufferCreateInfo bufferCreateInfos {};
		bufferCreateInfos.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfos.size = m_size;
		bufferCreateInfos.queueFamilyIndexCount = queueFamilyIndices.size();
		bufferCreateInfos.pQueueFamilyIndices = queueFamilyIndices.data();
		bufferCreateInfos.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfos.usage = bufferUsages.find(m_type)->second;
		if (vkCreateBuffer(m_gpu->getDevice(), &bufferCreateInfos, nullptr, &m_buffer) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create buffer for resource '" + sl::utils::toString(m_type) + "'");

		VkMemoryRequirements memoryRequirements {};
		vkGetBufferMemoryRequirements(m_gpu->getDevice(), m_buffer, &memoryRequirements);

		std::optional<std::size_t> memory {m_allocator.allocate(memoryRequirements.size, memoryRequirements.alignment)};
		if (!memory)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't allocate memory for resource '" + sl::utils::toString(m_type) + "'");
		m_memory = *memory;

		if (vkBindBufferMemory(m_gpu->getDevice(), m_buffer, m_allocator.getMemory(), m_memory - 1) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't bind memory for resource '" + sl::utils::toString(m_type) + "'");

		return sl::Result::eSuccess;
	}


	template <sl::memory::gpu::IsAllocator Allocator>
	auto Resource<Allocator>::destroy() noexcept -> void {
		if (!isResourceBuffer(m_type))
			return;
		if (m_memory != 0)
			m_allocator.deallocate(m_memory, m_size);
		if (m_buffer != VK_NULL_HANDLE)
			vkDestroyBuffer(m_gpu->getDevice(), m_buffer, nullptr);
	}


	template <sl::memory::gpu::IsAllocator Allocator>
	auto Resource<Allocator>::getBuffer() const noexcept -> std::optional<VkBuffer> {
		if (!isResourceBuffer(m_type))
			return std::nullopt;
		return m_buffer;
	}


	template <sl::memory::gpu::IsAllocator Allocator>
	template <std::ranges::input_range Range>
	auto Resource<Allocator>::set(Range &&value) noexcept -> sl::Result {
		std::size_t valueSize {static_cast<std::size_t> (std::ranges::distance(value))};
		sl::utils::Bytes valueMemorySize {valueSize * sizeof(decltype(*value.begin()))};
		if (valueMemorySize != m_size)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't set resource as value don't have the right size");

		void *resourceData {};
		if (vkMapMemory(m_gpu->getDevice(), m_allocator.getMemory(), m_memory - 1, m_size, 0, &resourceData) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't map resource memory");
		sl::utils::Janitor janitor {[this](){
			vkUnmapMemory(this->m_gpu->getDevice(), this->m_allocator.getMemory());
		}};

		if constexpr (std::ranges::contiguous_range<Range>) {
			(void)std::memcpy(resourceData, &*value.begin(), valueMemorySize);
		}
		else {
			std::size_t i {0};
			for (const auto &val : value) {
				reinterpret_cast<decltype(&*value.begin())> (resourceData)[i] = val;
				++i;
			}
		}

		return sl::Result::eSuccess;
	}

} // namespace sl::render::vulkan
