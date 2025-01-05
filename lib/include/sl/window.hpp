#pragma once

#include <turbolin/turbolin.hpp>

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
	};

	class SL_CORE Window final {
		public:
			Window() noexcept;
			~Window() = default;

			auto create(const WindowCreateInfos &createInfos) noexcept -> sl::Result;
			auto destroy() noexcept -> void;

		private:
			std::pmr::polymorphic_allocator<WindowImplementation> m_allocator;
			WindowImplementation *m_implementation;
	};

} // namespace sl
