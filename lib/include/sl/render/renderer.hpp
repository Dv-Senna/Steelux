#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/render/vulkan/instance.hpp"
#include "sl/result.hpp"
#include "sl/utils/string.hpp"
#include "sl/utils/utils.hpp"
#include "sl/window.hpp"


namespace sl::render {
	struct RendererCreateInfos {
		sl::String appName;
		sl::utils::Version appVersion;
		sl::Window *window;
	};

	class SL_CORE Renderer final {
		public:
			Renderer() noexcept = default;
			~Renderer() = default;

			auto create(const RendererCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

		private:
			sl::render::vulkan::Instance m_instance;
	};

} // namespace sl::render
