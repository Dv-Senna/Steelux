#include "sl/application.hpp"



namespace sl {
	sl::Result Application::create() noexcept {
		return this->onCreation();
	}


	void Application::destroy() noexcept {
		this->onDestruction();
	}

} // namespace sl
