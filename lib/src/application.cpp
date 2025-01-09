#include "sl/application.hpp"

#include "sl/inputManager.hpp"
#include "sl/utils/errorStack.hpp"
#include "sl/utils/time.hpp"



namespace sl {
	auto Application::create() noexcept -> sl::Result {
		m_targetDt = 1.f / m_infos.fps;

		sl::WindowCreateInfos windowCreateInfos {};
		windowCreateInfos.title = m_infos.title;
		windowCreateInfos.size = {16 * 70, 9 * 70};
		if (m_window.create(windowCreateInfos) != sl::Result::eSuccess)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create application's window");

		sl::render::RendererCreateInfos rendererCreateInfos {};
		rendererCreateInfos.window = &m_window;
		rendererCreateInfos.appName = m_infos.name;
		rendererCreateInfos.appVersion = m_infos.version;
		if (m_renderer.create(rendererCreateInfos) != sl::Result::eSuccess)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create application's renderer");

		return this->onCreation();
	}


	auto Application::destroy() noexcept -> void {
		this->onDestruction();
		m_renderer.destroy();
		m_window.destroy();
	}


	auto Application::mainloop() noexcept -> sl::Result {
		sl::utils::Millisecond dt {m_targetDt};
		sl::utils::TimePoint frameStart {sl::utils::TimePoint::now()};

		while (sl::InputManager::update()) {
			auto shouldContinueProgram {this->onUpdate(dt)};
			if (!shouldContinueProgram)
				return sl::Result::eFailure;
			if (!*shouldContinueProgram)
				return sl::Result::eSuccess;


			dt = sl::utils::TimePoint::now() - frameStart;
			if (dt < m_targetDt) {
				sl::utils::sleepFor(m_targetDt - dt);
				dt = m_targetDt;
			}
			frameStart = sl::utils::TimePoint::now();
		}

		return sl::Result::eSuccess;
	}


	auto Application::setTargetFps(sl::utils::PerSecond fps) noexcept -> void {
		m_infos.fps = fps;
		m_targetDt = 1.f / fps;
	}

} // namespace sl
