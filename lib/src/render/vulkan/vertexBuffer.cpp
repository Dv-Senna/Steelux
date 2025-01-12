#include "sl/render/vulkan/vertexBuffer.hpp"

#include <optional>

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/errorStack.hpp"


namespace sl::render::vulkan {
	auto findCompatibleMemoryType(std::uint32_t memoryFilter, VkMemoryPropertyFlags properties, auto memoryTypes) -> std::optional<std::uint32_t> {
		std::uint32_t i {};
		for (const VkMemoryType &type : memoryTypes) {
			if ((memoryFilter & (1 << i)) && (type.propertyFlags & properties) == properties)
				return i;
			++i;
		}

		return std::nullopt;
	}


	auto VertexBuffer::create(const VertexBufferCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_vertexComponentCount = createInfos.vertexComponentCount;
		m_verticesCount = createInfos.vertices.size() / m_vertexComponentCount;

		VkBufferCreateInfo bufferCreateInfos {};
		bufferCreateInfos.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfos.size = sizeof(float) * createInfos.vertices.size();
		bufferCreateInfos.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfos.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfos.queueFamilyIndexCount = 1;
		bufferCreateInfos.pQueueFamilyIndices = &m_gpu->getGraphicsQueue().familyIndex;
		if (vkCreateBuffer(m_gpu->getDevice(), &bufferCreateInfos, nullptr, &m_buffer) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create vertex buffer's buffer");

		VkMemoryRequirements memoryRequirements {};
		vkGetBufferMemoryRequirements(m_gpu->getDevice(), m_buffer, &memoryRequirements);

		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties {};
		vkGetPhysicalDeviceMemoryProperties(m_gpu->getPhysicalDevice(), &physicalDeviceMemoryProperties);

		auto memoryTypes {std::span(physicalDeviceMemoryProperties.memoryTypes, physicalDeviceMemoryProperties.memoryTypes + physicalDeviceMemoryProperties.memoryTypeCount)};
		std::optional<std::uint32_t> memoryTypeIndex {findCompatibleMemoryType(
			memoryRequirements.memoryTypeBits,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			memoryTypes
		)};
		if (!memoryTypeIndex)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't find compatible memory type");
		
		VkMemoryAllocateInfo memoryAllocateInfos {};
		memoryAllocateInfos.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfos.memoryTypeIndex = *memoryTypeIndex;
		memoryAllocateInfos.allocationSize = memoryRequirements.size;
		if (vkAllocateMemory(m_gpu->getDevice(), &memoryAllocateInfos, nullptr, &m_memory) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't allocate memory for vertex buffer");

		vkBindBufferMemory(m_gpu->getDevice(), m_buffer, m_memory, 0);


		void *data {};
		if (vkMapMemory(m_gpu->getDevice(), m_memory, 0, sizeof(float) * m_vertexComponentCount * m_verticesCount, 0, &data) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't map memory for vertex buffer");

		(void)std::memcpy(data, createInfos.vertices.data(), sizeof(float) * m_vertexComponentCount * m_verticesCount);
		vkUnmapMemory(m_gpu->getDevice(), m_memory);

		return sl::Result::eSuccess;
	}


	auto VertexBuffer::destroy() noexcept -> void {
		if (m_buffer != VK_NULL_HANDLE)
			vkDestroyBuffer(m_gpu->getDevice(), m_buffer, nullptr);
		if (m_memory != VK_NULL_HANDLE)
			vkFreeMemory(m_gpu->getDevice(), m_memory, nullptr);
	}

} // namespace sl::render::vulkan
