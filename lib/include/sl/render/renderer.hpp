#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/utils/string.hpp"
#include "sl/result.hpp"


namespace sl::render {
	struct RendererCreateInfos {
		sl::String appName;
	};

	class SL_CORE Renderer final {
		public:
			Renderer() noexcept = default;
			~Renderer() = default;

			auto create(const RendererCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;
	};

} // namespace sl::render
