#include "sl/linux/waylandWindow.hpp"

#ifdef SL_IMPLEMENT_WAYLAND

#include <cerrno>
#include <fcntl.h>
#include <sys/mman.h>
#include <ctime>
#include <unistd.h>

#include "sl/utils/logger.hpp"


namespace sl::linux_ {
	// code from https://wayland-book.com/surfaces/shared-memory.html
	static auto randName(char *buffer, std::size_t size) {
		struct timespec ts {};
		clock_gettime(CLOCK_REALTIME, &ts);
		long r {ts.tv_nsec};
		for (int i = 0; i < static_cast<long> (size); ++i) {
			buffer[i] = 'A' + (r & 0xf) + 2*(r & 0x10);
			r >>= 5;
		}
	}


	// code from https://wayland-book.com/surfaces/shared-memory.html
	static auto createShmFile() -> int {
		int retries {100};
		do {
			char name[] {"/wl_shm-XXXXXX"};
			randName(name + sizeof(name) - 7, 6);
			--retries;
			int fd {shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600)};
			if (fd >= 0) {
				shm_unlink(name);
				return fd;
			}
		} while (retries > 0 && errno == EEXIST);
		return -1;
	}


	// code from https://wayland-book.com/surfaces/shared-memory.html
	static auto allocateShmFile(std::size_t size) -> int {
		int fd {createShmFile()};
		if (fd < 0)
			return -1;

		int ret {};
		do {
			ret = ftruncate(fd, size);
		} while (ret < 0 && errno == EINTR);

		if (ret < 0) {
			close(fd);
			return -1;
		}
		return fd;
	}


	WaylandWindow::WaylandWindow() :
		m_state {}
	{

	}


	auto WaylandWindow::create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result {
		m_state.display = wl_display_connect(nullptr);
		if (m_state.display == nullptr)
			return sl::Result::eFailure;

		m_state.registry = wl_display_get_registry(m_state.display);
		if (m_state.registry == nullptr)
			return sl::Result::eFailure;


		const struct wl_registry_listener registryListener {
			.global = [](void *data, struct wl_registry *registry, uint32_t name, const char *interface, uint32_t version) -> void {
				State *state {reinterpret_cast<State*> (data)};
				if (std::strcmp(interface, wl_compositor_interface.name) == 0)
					state->compositor = reinterpret_cast<struct wl_compositor*> (wl_registry_bind(registry, name, &wl_compositor_interface, version));
				else if (std::strcmp(interface, wl_shm_interface.name) == 0)
					state->shm = reinterpret_cast<struct wl_shm*> (wl_registry_bind(registry, name, &wl_shm_interface, version));

				sl::mainLogger.debug("Interface : '{}', version : {}, name : {}", interface, version, name);
			},
			.global_remove = [](void *, struct wl_registry*, uint32_t /*name*/) -> void {}
		};


		(void)wl_registry_add_listener(m_state.registry, &registryListener, &this->m_state);
		(void)wl_display_roundtrip(m_state.display);

		if (m_state.compositor == nullptr)
			return sl::Result::eFailure;
		if (m_state.shm == nullptr)
			return sl::Result::eFailure;

		m_state.surface = wl_compositor_create_surface(m_state.compositor);
		if (m_state.surface == nullptr)
			return sl::Result::eFailure;

		constexpr std::size_t STRIDE {4};
		constexpr std::size_t BUFFER_COUNT {3};
		const std::size_t shmPoolSize {createInfos.size.w * createInfos.size.h * STRIDE * BUFFER_COUNT};

		int fd {allocateShmFile(shmPoolSize)};
		uint8_t *poolData {reinterpret_cast<uint8_t*> (mmap(nullptr, shmPoolSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0))};

		struct wl_shm_pool *pool {wl_shm_create_pool(m_state.shm, fd, shmPoolSize)};
		

		return sl::Result::eSuccess;
	}


	auto WaylandWindow::destroy() noexcept -> void {
		if (m_state.display != nullptr)
			wl_display_disconnect(m_state.display);
	}

} // namespace sl::linux_

#endif
