#pragma once

#ifndef SL_NO_SDL

#include <SDL3/SDL.h>

#include "sl/core.hpp"
#include "sl/window.hpp"


namespace sl {
	class SL_CORE SDLWindow final : public sl::WindowImplementation {
		public:
			SDLWindow() noexcept = default;
			~SDLWindow() override = default;

			auto create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result override;
			auto destroy() noexcept -> void override;
			auto createVkSurface(VkInstance instance) noexcept -> std::optional<VkSurfaceKHR> override;
			auto destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) noexcept -> void override;

			auto update() noexcept -> bool override;

			inline auto getSize() const noexcept -> const turbolin::Vec2i& override {return m_size;}

		private:
			SDL_Window *m_window;
			turbolin::Vec2i m_size;
	};

} // namespace sl

#define SL_IMPLEMENT_SDL

#endif
