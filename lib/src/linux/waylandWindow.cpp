#include "sl/linux/waylandWindow.hpp"

#ifdef SL_IMPLEMENT_WAYLAND

namespace sl::linux_ {
	WaylandWindow::WaylandWindow() :
		m_display {nullptr}
	{

	}


	auto WaylandWindow::create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result {
		m_display = wl_display_connect(nullptr);
		if (m_display == nullptr)
			return sl::Result::eFailure;

		return sl::Result::eSuccess;
	}


	auto WaylandWindow::destroy() noexcept -> void {
		if (m_display != nullptr)
			wl_display_disconnect(m_display);
	}

} // namespace sl::linux_

#endif
