#include "sl/render/vulkan/resource.hpp"

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/errorStack.hpp"


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


	auto Resource::create(const ResourceCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_type = createInfos.type;
		m_size = createInfos.size;

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

		return sl::Result::eSuccess;
	}


	auto Resource::destroy() noexcept -> void {
		if (!isResourceBuffer(m_type))
			return;
		vkDestroyBuffer(m_gpu->getDevice(), m_buffer, nullptr);
	}


	auto Resource::getBuffer() const noexcept -> std::optional<VkBuffer> {
		if (!isResourceBuffer(m_type))
			return std::nullopt;
		return m_buffer;
	}

} // namespace sl::render::vulkan
