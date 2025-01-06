#include "sl/SDLWindow.hpp"

#ifdef SL_IMPLEMENT_SDL

#include "sl/utils/errorStack.hpp"


namespace sl {
	using namespace sl::utils::literals;

	auto SDLWindow::create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result  {
		if (!SDL_Init(SDL_INIT_VIDEO))
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't init SDL3 : "_s + SDL_GetError());

		m_window = SDL_CreateWindow(createInfos.title.getData(), createInfos.size.w, createInfos.size.h, SDL_WINDOW_VULKAN);
		if (m_window == nullptr)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create an SDL3 window : "_s + SDL_GetError());

		return sl::utils::ErrorStack::push(sl::Result::eAllocationFailure, "Random allocation failure in window creation :)");
		return sl::Result::eSuccess;
	}


	auto SDLWindow::destroy() noexcept -> void  {
		if (m_window != nullptr)
			SDL_DestroyWindow(m_window);
	}

} // namespace sl

#endif
