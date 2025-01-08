#include "sl/inputManager.hpp"

#include "sl/eventManager.hpp"
#include "sl/utils/assert.hpp"


namespace sl {
	auto InputManager::linkWindow(sl::Window &window) noexcept -> void {
		using namespace sl::literals;

		s_window = &window;

		sl::EventFilter filter {
			.categories = {},
			.excludeCategories = {},
			.sources = {sl::UUID()},
			.excludeSources = {}
		};

		filter.categories = {"__sl_keydown"_ecat};
		(void)sl::EventManager::addListener<sl::Key> (filter, [&](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<Key> &event) noexcept -> void {
			s_keyStates[event.data] = true;
		});

		filter.categories = {"__sl_keyup"_ecat};
		(void)sl::EventManager::addListener<sl::Key> (filter, [&](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<Key> &event) noexcept -> void {
			s_keyStates[event.data] = false;
		});

		filter.categories = {"__sl_mousebuttondown"_ecat};
		(void)sl::EventManager::addListener<sl::MouseButton> (filter, [&](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<MouseButton> &event) noexcept -> void {
			s_mouseButtonStates[event.data] = true;
		});

		filter.categories = {"__sl_mousebuttonup"_ecat};
		(void)sl::EventManager::addListener<sl::MouseButton> (filter, [&](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<MouseButton> &event) noexcept -> void {
			s_mouseButtonStates[event.data] = false;
		});

		filter.categories = {"__sl_mousemotion"_ecat};
		(void)sl::EventManager::addListener<turbolin::Vec2f> (filter, [&](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<turbolin::Vec2f> &event) noexcept -> void {
			s_mouseMotion = event.data - s_mousePosition;
			s_mousePosition = event.data;
		});

		filter.categories = {"__sl_windowresize"_ecat};
		(void)sl::EventManager::addListener<turbolin::Vec2i> (filter, [&](const std::set<sl::EventCategory> &, sl::UUID, const sl::Event<turbolin::Vec2i> &) noexcept -> void {
			s_isWindowResized = true;
		});
	}


	auto InputManager::update() noexcept -> bool {
		SL_TEXT_ASSERT(s_window != nullptr, "A window must be linked to the InputManager before trying to update it");

		s_mouseMotion = {0.f, 0.f};
		s_isWindowResized = false;
		s_oldKeyStates = s_keyStates;
		s_oldMouseButtonStates = s_mouseButtonStates;

		s_running = s_window->update();

		for (const auto &key : s_keyStates) {
			if (!key.second) {
				sl::EventManager::send<sl::Key> ({KEY_UP}, sl::UUID(), {key.first});
				auto old {s_oldKeyStates.find(key.first)};
				if (old == s_oldKeyStates.end() || old->second)
					sl::EventManager::send<sl::Key> ({KEY_JUST_RELEASED}, sl::UUID(), {key.first});
				continue;
			}

			sl::EventManager::send<sl::Key> ({KEY_DOWN}, sl::UUID(), {key.first});
			auto old {s_oldKeyStates.find(key.first)};
			if (old == s_oldKeyStates.end() || !old->second)
				sl::EventManager::send<sl::Key> ({KEY_JUST_PRESSED}, sl::UUID(), {key.first});
		}

		for (const auto &button : s_mouseButtonStates) {
			if (!button.second) {
				sl::EventManager::send<sl::MouseButton> ({MOUSE_BUTTON_UP}, sl::UUID(), {button.first});
				auto old {s_oldMouseButtonStates.find(button.first)};
				if (old == s_oldMouseButtonStates.end() || old->second)
					sl::EventManager::send<sl::MouseButton> ({MOUSE_BUTTON_JUST_RELEASED}, sl::UUID(), {button.first});
				continue;
			}

			sl::EventManager::send<sl::MouseButton> ({MOUSE_BUTTON_DOWN}, sl::UUID(), {button.first});
			auto old {s_oldMouseButtonStates.find(button.first)};
			if (old == s_oldMouseButtonStates.end() || !old->second)
				sl::EventManager::send<sl::MouseButton> ({MOUSE_BUTTON_JUST_PRESSED}, sl::UUID(), {button.first});
		}

		if (s_mouseMotion != turbolin::Vec2f(0.f, 0.f))
			sl::EventManager::send<sl::MouseMotion> ({MOUSE_MOTION}, sl::UUID(), {{s_mousePosition, s_mouseMotion}});

		if (s_isWindowResized)
			sl::EventManager::send<turbolin::Vec2i> ({WINDOW_RESIZE}, sl::UUID(), {s_window->getSize()});

		return s_running;
	}


	auto InputManager::isKeyDown(sl::Key key) noexcept -> bool {
		auto it {s_keyStates.find(key)};
		if (it == s_keyStates.end())
			return false;
		return it->second;
	}


	auto InputManager::isKeyUp(sl::Key key) noexcept -> bool {
		auto it {s_keyStates.find(key)};
		if (it == s_keyStates.end())
			return false;
		return !it->second;
	}


	auto InputManager::isKeyJustPressed(sl::Key key) noexcept -> bool {
		if (!InputManager::isKeyDown(key))
			return false;
		auto it {s_oldKeyStates.find(key)};
		if (it == s_oldKeyStates.end())
			return true;
		return !it->second;
	}


	auto InputManager::isKeyJustReleased(sl::Key key) noexcept -> bool {
		if (!InputManager::isKeyUp(key))
			return false;
		auto it {s_oldKeyStates.find(key)};
		if (it == s_oldKeyStates.end())
			return true;
		return it->second;
	}


	auto InputManager::isMouseButtonDown(sl::MouseButton button) noexcept -> bool {
		auto it {s_mouseButtonStates.find(button)};
		if (it == s_mouseButtonStates.end())
			return false;
		return it->second;
	}


	auto InputManager::isMouseButtonUp(sl::MouseButton button) noexcept -> bool {
		auto it {s_mouseButtonStates.find(button)};
		if (it == s_mouseButtonStates.end())
			return false;
		return !it->second;
	}


	auto InputManager::isMouseButtonJustPressed(sl::MouseButton button) noexcept -> bool {
		if (!InputManager::isMouseButtonDown(button))
			return false;
		auto it {s_oldMouseButtonStates.find(button)};
		if (it == s_oldMouseButtonStates.end())
			return true;
		return !it->second;
	}


	auto InputManager::isMouseButtonJustReleased(sl::MouseButton button) noexcept -> bool {
		if (!InputManager::isMouseButtonUp(button))
			return false;
		auto it {s_oldMouseButtonStates.find(button)};
		if (it == s_oldMouseButtonStates.end())
			return true;
		return it->second;
	}


	auto InputManager::getWindowSize() noexcept -> turbolin::Vec2i {
		SL_TEXT_ASSERT(s_window != nullptr, "Can't get window size from InputManager if no window was linked");
		return s_window->getSize();
	}


	bool InputManager::s_running {true};
	bool InputManager::s_isWindowResized {false};
	sl::Window *InputManager::s_window {nullptr};
	std::map<Key, bool> InputManager::s_keyStates {};
	std::map<Key, bool> InputManager::s_oldKeyStates {};
	std::map<MouseButton, bool> InputManager::s_mouseButtonStates {};
	std::map<MouseButton, bool> InputManager::s_oldMouseButtonStates {};
	turbolin::Vec2f InputManager::s_mousePosition {0, 0};
	turbolin::Vec2f InputManager::s_mouseMotion {0, 0};

} // namespace sl
