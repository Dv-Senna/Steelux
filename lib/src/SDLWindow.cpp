#include "sl/SDLWindow.hpp"

#ifdef SL_IMPLEMENT_SDL

namespace sl {
	auto SDLWindow::create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result  {
		if (!SDL_Init(SDL_INIT_VIDEO))
			return sl::Result::eFailure;

		m_window = SDL_CreateWindow(createInfos.title.getData(), createInfos.size.w, createInfos.size.h, SDL_WINDOW_VULKAN);
		if (m_window == nullptr)
			return sl::Result::eFailure;
		return sl::Result::eSuccess;
	}


	auto SDLWindow::destroy() noexcept -> void  {
		if (m_window != nullptr)
			SDL_DestroyWindow(m_window);
	}

} // namespace sl

#endif
