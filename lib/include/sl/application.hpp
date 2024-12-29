#pragma once

#include <string>

#include "sl/core.hpp"
#include "sl/result.hpp"



namespace sl {
	class SL_CORE Application {
		public:
			struct Infos {
				std::string name;
			};

			Application() noexcept = default;
			virtual ~Application() = default;

			sl::Result create() noexcept;
			void destroy() noexcept;

			virtual sl::Result onCreation() noexcept = 0;
			virtual void onDestruction() noexcept = 0;


		protected:
			sl::Application::Infos m_infos;
	};

} // namespace sl
