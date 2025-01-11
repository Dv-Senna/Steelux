#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/enums.hpp"


namespace sl::render::vulkan {
	using namespace sl::utils::literals;
	class Instance;
	class GPU;

	enum class ShaderType : sl::utils::PackedEnumString {
		eFragment = "fragment"_pes,
		eVertex = "vertex"_pes,
		eCompute = "compute"_pes,
		eGeometry = "geometry"_pes
	};

	struct ShaderCreateInfos {
		ShaderType type;
		std::vector<std::byte> spirv;
		sl::render::vulkan::Instance *instance;
	};

	class SL_CORE Shader {
		public:
			Shader() noexcept = default;
			~Shader() = default;

			auto create(const ShaderCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			ShaderType m_type;
			VkShaderModule m_shaderModule;
	};

} // namespace sl::render::vulkan
