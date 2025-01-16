#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"


namespace sl::render::vulkan {
	struct RenderPassCreateInfos {

	};


	class SL_CORE RenderPass {
		public:
			RenderPass() noexcept = default;
			~RenderPass() = default;

			auto create(const RenderPassCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

		private:

	};

} // namespace sl::render::vulkan
