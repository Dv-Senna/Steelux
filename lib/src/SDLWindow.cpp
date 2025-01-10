#include "sl/SDLWindow.hpp"

#ifdef SL_IMPLEMENT_SDL

#include <map>

#include <SDL3/SDL_vulkan.h>

#include "sl/eventManager.hpp"
#include "sl/inputManager.hpp"
#include "sl/utils/errorStack.hpp"


namespace sl {
	using namespace sl::utils::literals;

	auto SDLWindow::create(const sl::WindowCreateInfos &createInfos) noexcept -> sl::Result  {
		if (!SDL_Init(SDL_INIT_VIDEO))
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't init SDL3 : "_s + SDL_GetError());

		m_window = SDL_CreateWindow(createInfos.title.getData(), createInfos.size.w, createInfos.size.h, SDL_WINDOW_VULKAN);
		if (m_window == nullptr)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create an SDL3 window : "_s + SDL_GetError());

		m_size = createInfos.size;
		return sl::Result::eSuccess;
	}


	auto SDLWindow::destroy() noexcept -> void  {
		m_size = {0, 0};
		if (m_window != nullptr)
			SDL_DestroyWindow(m_window);
	}


	auto SDLWindow::createVkSurface(VkInstance instance) noexcept -> std::optional<VkSurfaceKHR> {
		VkSurfaceKHR surface {VK_NULL_HANDLE};
		if (!SDL_Vulkan_CreateSurface(m_window, instance, nullptr, &surface))
			return sl::utils::ErrorStack::push(std::nullopt, "Can't create vulkan surface from SDL3 window : "_s + SDL_GetError());
		return surface;
	}


	auto SDLWindow::destroyVkSurface(VkInstance instance, VkSurfaceKHR surface) noexcept -> void {
		SDL_Vulkan_DestroySurface(instance, surface, nullptr);
	}


	auto SDLWindow::update() noexcept -> bool {
		using namespace sl::literals;

		static const std::map<SDL_Scancode, sl::Key> keys {
			{SDL_SCANCODE_A, sl::Key::eA},
			{SDL_SCANCODE_B, sl::Key::eB},
			{SDL_SCANCODE_C, sl::Key::eC},
			{SDL_SCANCODE_D, sl::Key::eD},
			{SDL_SCANCODE_E, sl::Key::eE},
			{SDL_SCANCODE_F, sl::Key::eF},
			{SDL_SCANCODE_G, sl::Key::eG},
			{SDL_SCANCODE_H, sl::Key::eH},
			{SDL_SCANCODE_I, sl::Key::eI},
			{SDL_SCANCODE_J, sl::Key::eJ},
			{SDL_SCANCODE_K, sl::Key::eK},
			{SDL_SCANCODE_L, sl::Key::eL},
			{SDL_SCANCODE_M, sl::Key::eM},
			{SDL_SCANCODE_N, sl::Key::eN},
			{SDL_SCANCODE_O, sl::Key::eO},
			{SDL_SCANCODE_P, sl::Key::eP},
			{SDL_SCANCODE_Q, sl::Key::eQ},
			{SDL_SCANCODE_R, sl::Key::eR},
			{SDL_SCANCODE_S, sl::Key::eS},
			{SDL_SCANCODE_T, sl::Key::eT},
			{SDL_SCANCODE_U, sl::Key::eU},
			{SDL_SCANCODE_V, sl::Key::eV},
			{SDL_SCANCODE_W, sl::Key::eW},
			{SDL_SCANCODE_X, sl::Key::eX},
			{SDL_SCANCODE_Y, sl::Key::eY},
			{SDL_SCANCODE_Z, sl::Key::eZ},
			{SDL_SCANCODE_0, sl::Key::e0},
			{SDL_SCANCODE_1, sl::Key::e1},
			{SDL_SCANCODE_2, sl::Key::e2},
			{SDL_SCANCODE_3, sl::Key::e3},
			{SDL_SCANCODE_4, sl::Key::e4},
			{SDL_SCANCODE_5, sl::Key::e5},
			{SDL_SCANCODE_6, sl::Key::e6},
			{SDL_SCANCODE_7, sl::Key::e7},
			{SDL_SCANCODE_8, sl::Key::e8},
			{SDL_SCANCODE_9, sl::Key::e9},
			{SDL_SCANCODE_ESCAPE, sl::Key::eEscape},
			{SDL_SCANCODE_TAB,    sl::Key::eTab},
			{SDL_SCANCODE_LSHIFT, sl::Key::eLShift},
			{SDL_SCANCODE_RSHIFT, sl::Key::eRShift},
			{SDL_SCANCODE_LCTRL,  sl::Key::eLCtrl},
			{SDL_SCANCODE_RCTRL,  sl::Key::eRCtrl},
			{SDL_SCANCODE_LALT,   sl::Key::eLAlt},
			{SDL_SCANCODE_RALT,   sl::Key::eRAlt},
			{SDL_SCANCODE_SPACE,  sl::Key::eSpace},
			{SDL_SCANCODE_RETURN, sl::Key::eReturn},
			{SDL_SCANCODE_BACKSPACE, sl::Key::eBackspace},
			{SDL_SCANCODE_LEFT,  sl::Key::eLeft},
			{SDL_SCANCODE_RIGHT, sl::Key::eRight},
			{SDL_SCANCODE_UP,    sl::Key::eUp},
			{SDL_SCANCODE_DOWN,  sl::Key::eDown},
			{SDL_SCANCODE_F1,  sl::Key::eF1},
			{SDL_SCANCODE_F2,  sl::Key::eF2},
			{SDL_SCANCODE_F3,  sl::Key::eF3},
			{SDL_SCANCODE_F4,  sl::Key::eF4},
			{SDL_SCANCODE_F5,  sl::Key::eF5},
			{SDL_SCANCODE_F6,  sl::Key::eF6},
			{SDL_SCANCODE_F7,  sl::Key::eF7},
			{SDL_SCANCODE_F8,  sl::Key::eF8},
			{SDL_SCANCODE_F9,  sl::Key::eF9},
			{SDL_SCANCODE_F10, sl::Key::eF10},
			{SDL_SCANCODE_F11, sl::Key::eF11},
			{SDL_SCANCODE_F12, sl::Key::eF12},
		};

		static const std::map<Uint8, sl::MouseButton> mouseButtons {
			{SDL_BUTTON_LEFT,   sl::MouseButton::eLeft},
			{SDL_BUTTON_RIGHT,  sl::MouseButton::eRight},
			{SDL_BUTTON_MIDDLE, sl::MouseButton::eMiddle},
			{SDL_BUTTON_X1,     sl::MouseButton::eX1},
			{SDL_BUTTON_X2,     sl::MouseButton::eX2},
		};


		SDL_Event event {};
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_EVENT_KEY_DOWN: {
					auto it {keys.find(event.key.scancode)};
					sl::Key key {};
					if (it == keys.end())
						key = sl::Key::eUnknown;
					else
						key = it->second;
					sl::EventManager::send<sl::Key> ({"__sl_keydown"_ecat}, sl::UUID(), {key});
					break;
				}
				case SDL_EVENT_KEY_UP: {
					auto it {keys.find(event.key.scancode)};
					sl::Key key {};
					if (it == keys.end())
						key = sl::Key::eUnknown;
					else
						key = it->second;
					sl::EventManager::send<sl::Key> ({"__sl_keyup"_ecat}, sl::UUID(), {key});
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_DOWN: {
					auto it {mouseButtons.find(event.button.button)};
					sl::MouseButton key {};
					if (it == mouseButtons.end())
						key = sl::MouseButton::eUnknown;
					else
						key = it->second;
					sl::EventManager::send<sl::MouseButton> ({"__sl_mousebuttondown"_ecat}, sl::UUID(), {key});
					break;
				}
				case SDL_EVENT_MOUSE_BUTTON_UP: {
					auto it {mouseButtons.find(event.button.button)};
					sl::MouseButton key {};
					if (it == mouseButtons.end())
						key = sl::MouseButton::eUnknown;
					else
						key = it->second;
					sl::EventManager::send<sl::MouseButton> ({"__sl_mousebuttonup"_ecat}, sl::UUID(), {key});
					break;
				}
				case SDL_EVENT_MOUSE_MOTION:
					sl::EventManager::send<turbolin::Vec2f> ({"__sl_mousemotion"_ecat}, sl::UUID(), {{event.motion.x, event.motion.y}});
					break;
				case SDL_EVENT_WINDOW_RESIZED:
					sl::EventManager::send<turbolin::Vec2i> ({"sl_windowresize"_ecat}, sl::UUID(), {{event.window.data1, event.window.data2}});
					break;
				case SDL_EVENT_QUIT:
					return false;
				default:
					break;
			}
		}

		return true;
	}

} // namespace sl

#endif
