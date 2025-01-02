#pragma once

#include "sl/core.hpp"
#include "sl/result.hpp"
#include "sl/utils/string.hpp"



namespace sl {
	class SL_CORE Application {
		public:
			struct Infos {
				sl::String name;
			};

			Application() noexcept = default;
			virtual ~Application() = default;

			auto create() noexcept -> sl::Result;
			auto destroy() noexcept -> void;

			virtual auto onCreation() noexcept -> sl::Result = 0;
			virtual auto onDestruction() noexcept -> void = 0;


		protected:
			sl::Application::Infos m_infos;
	};

} // namespace sl
