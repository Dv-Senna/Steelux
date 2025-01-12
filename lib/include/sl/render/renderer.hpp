#pragma once

#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/string.hpp"
#include "sl/utils/utils.hpp"
#include "sl/window.hpp"

#include "sl/render/vulkan/instance.hpp"
#include "sl/render/vulkan/swapchain.hpp"


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

			inline auto getInstance() noexcept -> sl::render::vulkan::Instance& {return m_instance;}
			inline auto getSwapchain() noexcept -> sl::render::vulkan::Swapchain& {return m_swapchain;}


		private:
			sl::render::vulkan::Instance m_instance;
			sl::render::vulkan::Swapchain m_swapchain;
	};

} // namespace sl::render
