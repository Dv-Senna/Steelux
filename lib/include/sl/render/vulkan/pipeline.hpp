#pragma once

#include <vector>

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"


namespace sl::render::vulkan {
	class Instance;
	class GPU;
	class Shader;

	enum class PipelineType {
		eGraphics,
		eCompute
	};

	struct PipelineCreateInfos {
		sl::render::vulkan::Instance *instance;
		PipelineType type;
		std::vector<sl::render::vulkan::Shader*> shaders;
	};

	class SL_CORE Pipeline {
		public:
			Pipeline() noexcept = default;
			~Pipeline() = default;

			auto create(const PipelineCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;


		private:
			sl::render::vulkan::Instance *m_instance;
			sl::render::vulkan::GPU *m_gpu;
			PipelineType m_type;
			VkPipelineLayout m_pipelineLayout;
			VkPipeline m_pipeline;
	};

} // namespace sl::render::vulkan
