#pragma once

#ifndef SL_NO_WAYLAND

#include <wayland-client.h>

#include "sl/window.hpp"


namespace sl::linux_ {
	class SL_CORE WaylandWindow final : public sl::WindowImplementation {
		public:
			WaylandWindow();
			~WaylandWindow() override = default;

			auto create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result override;
			auto destroy() noexcept -> void override;

		private:
			struct wl_display *m_display;
	};

} // sl::linux_

#define SL_IMPLEMENT_WAYLAND

#endif
