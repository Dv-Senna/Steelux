#pragma once

#if defined(SL_LINUX) && !defined(SL_NO_WAYLAND)

#include <wayland-client.h>
#include <xdg-shell/xdg-shell-client-protocol.h>

#include "sl/window.hpp"


namespace sl::linux_ {
	class SL_CORE WaylandWindow final : public sl::WindowImplementation {
		public:
			WaylandWindow();
			~WaylandWindow() override = default;

			auto create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result override;
			auto destroy() noexcept -> void override;

		private:
			static constexpr std::size_t BUFFER_COUNT {3};

			struct State {
				wl_display *display {nullptr};
				wl_registry *registry {nullptr};
				wl_compositor *compositor {nullptr};
				wl_surface *surface {nullptr};
				wl_shm *shm {nullptr};
				uint8_t *poolData {nullptr};
				wl_buffer *buffers[BUFFER_COUNT] {nullptr, nullptr, nullptr};
				std::size_t activeBufferIndex {0};
				xdg_wm_base *xdgWmBase {nullptr};
				xdg_toplevel *xdgTopLevel {nullptr};
				xdg_surface *xdgSurface {nullptr};
			};

			State m_state;
	};

} // sl::linux_

#define SL_IMPLEMENT_WAYLAND

#endif
