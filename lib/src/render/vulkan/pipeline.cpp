#include "sl/render/vulkan/pipeline.hpp"

#include <map>
#include <vector>

#include "sl/render/vulkan/GPU.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/render/vulkan/shader.hpp"
#include "sl/utils/assert.hpp"
#include "sl/utils/errorStack.hpp"


namespace sl::render::vulkan {
	auto Pipeline::create(const PipelineCreateInfos &createInfos) noexcept -> sl::Result {
		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_type = createInfos.type;

		SL_TEXT_ASSERT(m_type == sl::render::vulkan::PipelineType::eGraphics, "Only graphics pipeline is supported for now");

		std::vector<VkDynamicState> dynamicStates {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfos {};
		pipelineDynamicStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfos.dynamicStateCount = dynamicStates.size();
		pipelineDynamicStateCreateInfos.pDynamicStates = dynamicStates.data();

		std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions {
			VkVertexInputBindingDescription{
				.binding = 0,
				.stride = sizeof(float) * 5,
				.inputRate = VK_VERTEX_INPUT_RATE_VERTEX,
			}
		};

		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions {
			VkVertexInputAttributeDescription{
				.location = 0,
				.binding = 0,
				.format = VK_FORMAT_R8G8_SNORM,
				.offset = 0
			},
			VkVertexInputAttributeDescription{
				.location = 1,
				.binding = 0,
				.format = VK_FORMAT_R8G8B8_UNORM,
				.offset = sizeof(float) * 2
			}
		};

		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfos {};
		pipelineVertexInputStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfos.vertexAttributeDescriptionCount = vertexInputAttributeDescriptions.size();
		pipelineVertexInputStateCreateInfos.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
		pipelineVertexInputStateCreateInfos.vertexBindingDescriptionCount = vertexInputBindingDescriptions.size();
		pipelineVertexInputStateCreateInfos.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();

		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfos {};
		pipelineViewportStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfos.scissorCount = 1;
		pipelineViewportStateCreateInfos.pScissors = nullptr;
		pipelineViewportStateCreateInfos.viewportCount = 1;
		pipelineViewportStateCreateInfos.pViewports = nullptr;

		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState {};
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		pipelineColorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		pipelineColorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE;
		pipelineColorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;

		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfos {};
		pipelineColorBlendStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfos.logicOpEnable = VK_FALSE;
		pipelineColorBlendStateCreateInfos.logicOp = VK_LOGIC_OP_NO_OP;
		pipelineColorBlendStateCreateInfos.blendConstants[0] = 0.f;
		pipelineColorBlendStateCreateInfos.blendConstants[1] = 0.f;
		pipelineColorBlendStateCreateInfos.blendConstants[2] = 0.f;
		pipelineColorBlendStateCreateInfos.blendConstants[3] = 0.f;
		pipelineColorBlendStateCreateInfos.attachmentCount = 1;
		pipelineColorBlendStateCreateInfos.pAttachments = &pipelineColorBlendAttachmentState;

		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfos {};
		pipelineMultisampleStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfos.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipelineMultisampleStateCreateInfos.sampleShadingEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfos.minSampleShading = 1;
		pipelineMultisampleStateCreateInfos.alphaToCoverageEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfos.alphaToOneEnable = VK_FALSE;
		pipelineMultisampleStateCreateInfos.pSampleMask = nullptr;

		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfos {};
		pipelineDepthStencilStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfos.depthTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfos.depthWriteEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfos.stencilTestEnable = VK_FALSE;
		pipelineDepthStencilStateCreateInfos.depthBoundsTestEnable = VK_FALSE;

		VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfos {};
		pipelineTessellationStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		pipelineTessellationStateCreateInfos.patchControlPoints = 0;

		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfos {};
		pipelineInputAssemblyStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfos.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		pipelineInputAssemblyStateCreateInfos.primitiveRestartEnable = VK_FALSE;

		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfos {};
		pipelineRasterizationStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfos.cullMode = VK_CULL_MODE_NONE;
		pipelineRasterizationStateCreateInfos.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		pipelineRasterizationStateCreateInfos.lineWidth = 1.f;
		pipelineRasterizationStateCreateInfos.polygonMode = VK_POLYGON_MODE_FILL;
		pipelineRasterizationStateCreateInfos.depthBiasEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfos.depthClampEnable = VK_FALSE;
		pipelineRasterizationStateCreateInfos.depthBiasClamp = 0.f;
		pipelineRasterizationStateCreateInfos.depthBiasSlopeFactor = 0.f;
		pipelineRasterizationStateCreateInfos.depthBiasConstantFactor = 0.f;
		pipelineRasterizationStateCreateInfos.rasterizerDiscardEnable = VK_FALSE;

		std::vector<VkPipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos {};
		pipelineShaderStageCreateInfos.reserve(createInfos.shaders.size());
		for (const auto &shader : createInfos.shaders) {
			static const std::map<sl::render::vulkan::ShaderType, VkShaderStageFlagBits> s_shaderStages {
				{sl::render::vulkan::ShaderType::eVertex, VK_SHADER_STAGE_VERTEX_BIT},
				{sl::render::vulkan::ShaderType::eCompute, VK_SHADER_STAGE_COMPUTE_BIT},
				{sl::render::vulkan::ShaderType::eFragment, VK_SHADER_STAGE_FRAGMENT_BIT},
				{sl::render::vulkan::ShaderType::eGeometry, VK_SHADER_STAGE_GEOMETRY_BIT},
			};

			VkPipelineShaderStageCreateInfo shaderStageCreateInfos {};
			shaderStageCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStageCreateInfos.stage = s_shaderStages.find(shader->getType())->second;
			shaderStageCreateInfos.module = shader->getShaderModule();
			shaderStageCreateInfos.pSpecializationInfo = nullptr;
			shaderStageCreateInfos.pName = "main";

			pipelineShaderStageCreateInfos.push_back(shaderStageCreateInfos);
		}

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfos {};
		pipelineLayoutCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfos.setLayoutCount = 0;
		pipelineLayoutCreateInfos.pushConstantRangeCount = 0;
		if (vkCreatePipelineLayout(m_gpu->getDevice(), &pipelineLayoutCreateInfos, nullptr, &m_pipelineLayout) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create graphics pipeline layout");

		VkFormat colorAttachmentFormat {VK_FORMAT_B8G8R8A8_SRGB};
		VkPipelineRenderingCreateInfo pipelineRenderingCreateInfos {};
		pipelineRenderingCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
		pipelineRenderingCreateInfos.colorAttachmentCount = 1;
		pipelineRenderingCreateInfos.pColorAttachmentFormats = &colorAttachmentFormat;

		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfos {};
		graphicsPipelineCreateInfos.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfos.pNext = &pipelineRenderingCreateInfos;
		graphicsPipelineCreateInfos.stageCount = pipelineShaderStageCreateInfos.size();
		graphicsPipelineCreateInfos.pStages = pipelineShaderStageCreateInfos.data();
		graphicsPipelineCreateInfos.pDynamicState = &pipelineDynamicStateCreateInfos;
		graphicsPipelineCreateInfos.pViewportState = &pipelineViewportStateCreateInfos;
		graphicsPipelineCreateInfos.pColorBlendState = &pipelineColorBlendStateCreateInfos;
		graphicsPipelineCreateInfos.pMultisampleState = &pipelineMultisampleStateCreateInfos;
		graphicsPipelineCreateInfos.pVertexInputState = &pipelineVertexInputStateCreateInfos;
		graphicsPipelineCreateInfos.pDepthStencilState = &pipelineDepthStencilStateCreateInfos;
		graphicsPipelineCreateInfos.pTessellationState = &pipelineTessellationStateCreateInfos;
		graphicsPipelineCreateInfos.pInputAssemblyState = &pipelineInputAssemblyStateCreateInfos;
		graphicsPipelineCreateInfos.pRasterizationState = &pipelineRasterizationStateCreateInfos;
		graphicsPipelineCreateInfos.renderPass = VK_NULL_HANDLE;
		graphicsPipelineCreateInfos.subpass = 0;
		graphicsPipelineCreateInfos.layout = m_pipelineLayout;
		graphicsPipelineCreateInfos.basePipelineIndex = 0;
		graphicsPipelineCreateInfos.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_gpu->getDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfos, nullptr, &m_pipeline) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create graphics pipeline");
		return sl::Result::eSuccess;
	}


	auto Pipeline::destroy() noexcept -> void {
		if (m_pipeline != VK_NULL_HANDLE)
			vkDestroyPipeline(m_gpu->getDevice(), m_pipeline, nullptr);
		if (m_pipelineLayout != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(m_gpu->getDevice(), m_pipelineLayout, nullptr);
	}

} // namespace sl::render::vulkan
