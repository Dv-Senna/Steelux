#pragma once

#include <map>

#include <turbolin/turbolin.hpp>

#include "sl/core.hpp"
#include "sl/utils/enums.hpp"
#include "sl/window.hpp"


namespace sl {
	using namespace sl::utils::literals;

	enum class Key : sl::utils::PackedEnumString {
		eA = "A"_pes, eB = "B"_pes, eC = "C"_pes, eD = "D"_pes, eE = "E"_pes, eF = "F"_pes,
		eG = "G"_pes, eH = "H"_pes, eI = "I"_pes, eJ = "J"_pes, eK = "K"_pes, eL = "L"_pes,
		eM = "M"_pes, eN = "N"_pes, eO = "O"_pes, eP = "P"_pes, eQ = "Q"_pes, eR = "R"_pes,
		eS = "S"_pes, eT = "T"_pes, eU = "U"_pes, eV = "V"_pes, eW = "W"_pes, eX = "X"_pes,
		eY = "Y"_pes, eZ = "Z"_pes,

		e0 = "0"_pes, e1 = "1"_pes, e2 = "2"_pes, e3 = "3"_pes, e4 = "4"_pes, e5 = "5"_pes,
		e6 = "6"_pes, e7 = "7"_pes, e8 = "8"_pes, e9 = "9"_pes,

		eEscape = "escape"_pes,
		eTab = "tab"_pes,
		eLShift = "lshift"_pes,
		eRShift = "rshift"_pes,
		eLCtrl = "lctrl"_pes,
		eRCtrl = "rctrl"_pes,
		eLAlt = "lalt"_pes,
		eRAlt = "ralt"_pes,
		eSpace = "space"_pes,
		eReturn = "return"_pes,
		eBackspace = "backspace"_pes,

		eLeft = "left"_pes,
		eRight = "right"_pes,
		eUp = "up"_pes,
		eDown = "down"_pes,

		eF1 = "F1"_pes, eF2 = "F2"_pes, eF3 = "F3"_pes, eF4 = "F4"_pes, eF5 = "F5"_pes, eF6 = "F6"_pes,
		eF7 = "F7"_pes, eF8 = "F8"_pes, eF9 = "F9"_pes, eF10 = "F10"_pes, eF11 = "F11"_pes, eF12 = "F12"_pes,

		eUnknown = "unknown"_pes
	};


	enum class MouseButton  : sl::utils::PackedEnumString{
		eLeft = "left"_pes,
		eRight = "right"_pes,
		eMiddle = "middle"_pes,
		eX1 = "X1"_pes,
		eX2 = "X2"_pes,
		eUnknown = "unknown"_pes
	};


	class SL_CORE InputManager {
		public:
			InputManager() = delete;

			static auto linkWindow(sl::Window &window) noexcept -> void;
			static auto update() noexcept -> bool;

			inline static auto isRunning() noexcept -> bool {return s_running;}

			static auto isKeyDown(sl::Key key) noexcept -> bool;
			static auto isKeyUp(sl::Key key) noexcept -> bool;
			static auto isKeyJustPressed(sl::Key key) noexcept -> bool;
			static auto isKeyJustReleased(sl::Key key) noexcept -> bool;

			static auto isMouseButtonDown(sl::MouseButton button) noexcept -> bool;
			static auto isMouseButtonUp(sl::MouseButton button) noexcept -> bool;
			static auto isMouseButtonJustPressed(sl::MouseButton button) noexcept -> bool;
			static auto isMouseButtonJustDown(sl::MouseButton button) noexcept -> bool;

			inline static auto getMousePosition() noexcept -> const turbolin::Vec2f& {return s_mousePosition;}
			inline static auto getMouseMotion() noexcept -> const turbolin::Vec2f& {return s_mouseMotion;}

			inline static auto isWindowResize() noexcept -> bool {return s_isWindowResized;}
			static auto getWindowSize() noexcept -> turbolin::Vec2i;


		private:
			static bool s_running;
			static bool s_isWindowResized;
			static sl::Window *s_window;

			static std::map<Key, bool> s_keyStates;
			static std::map<Key, bool> s_oldKeyStates;
			static std::map<MouseButton, bool> s_mouseButtonStates;
			static std::map<MouseButton, bool> s_oldMouseButtonStates;
			static turbolin::Vec2f s_mousePosition;
			static turbolin::Vec2f s_mouseMotion;
	};

} // namespace sl
