#pragma once

#include <expected>

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/string.hpp"
#include "sl/utils/units.hpp"
#include "sl/window.hpp"



namespace sl {
	class SL_CORE Application {
		public:
			struct Infos {
				sl::String name;
				sl::String title;
				sl::utils::PerSecond fps;
			};

			Application() noexcept = default;
			virtual ~Application() = default;

			auto create() noexcept -> sl::Result;
			auto destroy() noexcept -> void;
			auto mainloop() noexcept -> sl::Result;

			auto setTargetFps(sl::utils::PerSecond fps) noexcept -> void;

			virtual auto onCreation() noexcept -> sl::Result = 0;
			virtual auto onDestruction() noexcept -> void = 0;

			virtual auto onUpdate(sl::utils::Millisecond dt) noexcept -> std::expected<bool, sl::Result> = 0;


		protected:
			sl::Application::Infos m_infos;

		private:
			sl::Window m_window;
			sl::utils::Millisecond m_targetDt;
	};

} // namespace sl
