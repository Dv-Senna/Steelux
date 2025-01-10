#pragma once

#include <optional>

#include <turbolin/turbolin.hpp>
#include <vulkan/vulkan.h>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/string.hpp"


namespace sl {
	class WindowImplementation;

	struct WindowCreateInfos {
		std::pmr::polymorphic_allocator<WindowImplementation> allocator;
		sl::String title;
		turbolin::Vec2i size;
	};

	class WindowImplementation {
		public:
			WindowImplementation() noexcept = default;
			virtual ~WindowImplementation() = default;

			virtual auto create(const WindowCreateInfos &createInfos) noexcept -> sl::Result = 0;
			virtual auto destroy() noexcept -> void = 0;
			virtual auto createVkSurface(VkInstance instance) noexcept -> std::optional<VkSurfaceKHR> = 0;
			virtual auto destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) noexcept -> void = 0;

			virtual auto update() noexcept -> bool = 0;

			virtual auto getSize() const noexcept -> const turbolin::Vec2i& = 0;
	};

	class SL_CORE Window final {
		public:
			Window() noexcept;
			~Window() = default;

			auto create(const WindowCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;
			auto createVkSurface(VkInstance instance) noexcept -> std::optional<VkSurfaceKHR>;
			auto destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) noexcept -> void;

			/* This function send values to the following event endpoint :
			 *     - {"__sl_keydown"},         global, <sl::Key>
			 *     - {"__sl_keyup"},           global, <sl::Key>
			 *     - {"__sl_mousebuttondown"}, global, <sl::MouseButton>
			 *     - {"__sl_mousebuttondown"}, global, <sl::MouseButton>
			 *     - {"__sl_mousemotion"},     global, <turbolin::Vec2f>
			 *     - {"__sl_windowresize"},    global, <turbolin::Vec2i>
			 */
			auto update() noexcept -> bool;

			auto getSize() const noexcept -> turbolin::Vec2i;

		private:
			std::pmr::polymorphic_allocator<WindowImplementation> m_allocator;
			WindowImplementation *m_implementation;
	};

} // namespace sl
