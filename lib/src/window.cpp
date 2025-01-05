#include "sl/window.hpp"

#include "sl/linux/waylandWindow.hpp"


namespace sl {
	Window::Window() noexcept :
		m_allocator {},
		m_implementation {nullptr}
	{

	}


	auto Window::create(const WindowCreateInfos &createInfos) noexcept -> sl::Result {
	#ifdef SL_IMPLEMENT_WAYLAND
		m_implementation = m_allocator.new_object<sl::linux_::WaylandWindow> ();
		if (m_implementation->create(createInfos) != sl::Result::eSuccess)
			return sl::Result::eFailure;
	#endif
		return sl::Result::eSuccess;
	}


	auto Window::destroy() noexcept -> void {
		if (m_implementation == nullptr)
			return;

		m_implementation->destroy();
		m_allocator.delete_object(m_implementation);
	}

} // namespace sl
