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

		private:
			SDL_Window *m_window;
	};

} // namespace sl

#define SL_IMPLEMENT_SDL

#endif
