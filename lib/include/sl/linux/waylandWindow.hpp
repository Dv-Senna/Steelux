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
			struct State {
				struct wl_display *display {nullptr};
				struct wl_registry *registry {nullptr};
				struct wl_compositor *compositor {nullptr};
				struct wl_surface *surface {nullptr};
				struct wl_shm *shm {nullptr};
			};

			State m_state;
	};

} // sl::linux_

#define SL_IMPLEMENT_WAYLAND

#endif
