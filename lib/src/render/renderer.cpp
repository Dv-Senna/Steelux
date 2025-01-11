#include "sl/render/renderer.hpp"

#include "sl/utils/errorStack.hpp"


namespace sl::render {
	auto Renderer::create(const RendererCreateInfos &createInfos) noexcept -> sl::Result {
		sl::render::vulkan::InstanceCreateInfos instanceCreateInfos {};
		instanceCreateInfos.appName = createInfos.appName;
		instanceCreateInfos.appVersion = createInfos.appVersion;
		instanceCreateInfos.window = createInfos.window;
		if (m_instance.create(instanceCreateInfos) != sl::Result::eSuccess)
			return sl::utils::ErrorStack::push(sl::Result::eFailure, "Can't create instance of graphics api");

		sl::render::vulkan::SwapchainCreateInfos swapchainCreateInfos {};
		swapchainCreateInfos.instance = &m_instance;
		if (m_swapchain.create(swapchainCreateInfos) != sl::Result::eSuccess)
			return sl::ErrorStack::push(sl::Result::eFailure, "Can't create swapchain for renderer");

		return sl::Result::eSuccess;
	}


	auto Renderer::destroy() noexcept -> void {
		m_swapchain.destroy();
		m_instance.destroy();
	}


} // namespace sl::render
