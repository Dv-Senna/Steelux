#include "sl/window.hpp"

#include "sl/inputManager.hpp"
#include "sl/linux/waylandWindow.hpp"
#include "sl/SDLWindow.hpp"


namespace sl {
	Window::Window() noexcept :
		m_allocator {},
		m_implementation {nullptr}
	{

	}


	auto Window::create(const WindowCreateInfos &createInfos) noexcept -> sl::Result {
	#ifdef SL_IMPLEMENT_SDL
		m_implementation = m_allocator.new_object<sl::SDLWindow> ();
	#elifdef SL_IMPLEMENT_WAYLAND
		m_implementation = m_allocator.new_object<sl::linux_::WaylandWindow> ();
	#endif
		if (m_implementation->create(createInfos) != sl::Result::eSuccess)
			return sl::Result::eFailure;

		sl::InputManager::linkWindow(*this);
		return sl::Result::eSuccess;
	}


	auto Window::destroy() noexcept -> void {
		if (m_implementation == nullptr)
			return;

		m_implementation->destroy();
		m_allocator.delete_object(m_implementation);
	}


	auto Window::update() noexcept -> bool {
		if (m_implementation == nullptr)
			return false;
		return m_implementation->update();
	}


	auto Window::getSize() const noexcept -> turbolin::Vec2i {
		if (m_implementation == nullptr)
			return {0, 0};
		return m_implementation->getSize();
	}

} // namespace sl
