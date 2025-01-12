#include "sl/render/vulkan/pipeline.hpp"

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/assert.hpp"
#include "sl/utils/errorStack.hpp"


namespace sl::render::vulkan {
	auto Pipeline::create(const PipelineCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_type = createInfos.type;

		SL_TEXT_ASSERT(m_type != sl::render::vulkan::PipelineType::eGraphics, "Only graphics pipeline is supported for now");

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfos {};
		graphicsPipelineCreateInfos.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfos.layout = 0;
		graphicsPipelineCreateInfos.stageCount = 1;
		graphicsPipelineCreateInfos.pStages = nullptr;

		return sl::Result::eSuccess;
	}


	auto Pipeline::destroy() noexcept -> void {

	}

} // namespace sl::render::vulkan
