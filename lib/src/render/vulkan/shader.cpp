#include "sl/render/vulkan/shader.hpp"

#include "sl/render/vulkan/instance.hpp"
#include "sl/utils/assert.hpp"
#include "sl/utils/errorStack.hpp"


namespace sl::render::vulkan {
	auto Shader::create(const ShaderCreateInfos &createInfos) noexcept -> sl::Result {
		SL_TEXT_ASSERT(createInfos.spirv.size() % sizeof(std::uint32_t) == 0, "The spir-v code's size must be a multiple of sizeof(std::uint32_t)=4[B]");

		m_instance = createInfos.instance;
		m_gpu = m_instance->getGpu();
		m_type = createInfos.type;

		VkShaderModuleCreateInfo shaderModuleCreateInfos {};
		shaderModuleCreateInfos.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfos.codeSize = createInfos.spirv.size();
		shaderModuleCreateInfos.pCode = reinterpret_cast<const std::uint32_t*> (createInfos.spirv.data());
		if (vkCreateShaderModule(m_instance->getGpu()->getDevice(), &shaderModuleCreateInfos, nullptr, &m_shaderModule) != VK_SUCCESS)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create shader module");
		return sl::Result::eSuccess;
	}


	auto Shader::destroy() noexcept -> void {
		if (m_shaderModule != VK_NULL_HANDLE)
			vkDestroyShaderModule(m_gpu->getDevice(), m_shaderModule, nullptr);
	}

} // sl::render::vulkan
