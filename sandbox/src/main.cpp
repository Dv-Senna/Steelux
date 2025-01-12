#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <vulkan/vulkan.h>

#include <sl/utils/array.hpp>
#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>
#include <sl/utils/logger.hpp>
#include <sl/utils/units.hpp>
#include <sl/utils/hash.hpp>
#include <sl/utils/file.hpp>

#include <sl/memory/poolAllocator.hpp>
#include <sl/memory/heapAllocator.hpp>
#include <sl/memory/stackAllocator.hpp>
#include <sl/memory/doubleStackAllocator.hpp>
#include <sl/memory/singleFrameAllocator.hpp>
#include <sl/memory/doubleBufferedAllocator.hpp>

#include <sl/window.hpp>
#include <sl/eventManager.hpp>
#include <sl/inputManager.hpp>

#include <sl/render/vulkan/pipeline.hpp>
#include <sl/render/vulkan/shader.hpp>
#include <sl/render/vulkan/vertexBuffer.hpp>

#include <memory>
#include <print>



using namespace sl::utils::literals;


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			m_infos.name = "Steelux_sandbox";
			m_infos.title = "Steelux sandbox app";
			m_infos.version = "0.1.0"_ver;
			m_infos.fps = 60.0_hz;

			sl::memory::HeapAllocator heapAllocator {};
			sl::memory::HeapMemoryResource heapMemoryResource {heapAllocator};

			std::println("------------ PMR STRING -------------");
			sl::utils::BasicString<char, std::pmr::polymorphic_allocator<char>> str {"Hello World from Steelux !", {&heapMemoryResource}};
			std::println("str : {} ({}, {})", str, str.getSize(), str.getCapacity());

			sl::utils::BasicString<char, sl::memory::HeapAllocatorView<char>> str2 {"Hello World for a 2nd string from Steelux !", {heapAllocator}};
			std::println("str2 : {} ({}, {})", str2, str2.getSize(), str2.getCapacity());

			std::println("------------ HASH -------------");
			std::println("'Hello World !'_hash8 : {}", "Hello World !"_hash8);
			std::println("'Hello World !'_hash16 : {}", "Hello World !"_hash16);
			std::println("'Hello World !'_hash32 : {}", "Hello World !"_hash32);
			std::println("'Hello World !'_hash64 : {}", "Hello World !"_hash64);

			sl::String str3 {"Hello World !"};
			std::println("hash8(str) : {}", sl::utils::hash<sl::utils::Hash8> (str3));
			std::println("hash16(str) : {}", sl::utils::hash<sl::utils::Hash16> (str3));
			std::println("hash32(str) : {}", sl::utils::hash<sl::utils::Hash32> (str3));
			std::println("hash64(str) : {}", sl::utils::hash<sl::utils::Hash64> (str3));

		}

		~SandboxApp() override {

		}

		auto onCreation() noexcept -> sl::Result override {
			std::println("Creation");

			sl::render::vulkan::VertexBufferCreateInfos vertexBufferCreateInfos {};
			vertexBufferCreateInfos.instance = &m_renderer.getInstance();
			vertexBufferCreateInfos.vertices = {
				-0.5f, -0.5f,    1.f, 0.f, 0.f,
				0.f, 0.5f,       0.f, 1.f, 0.f,
				0.5f, -0.5f,     0.f, 0.f, 1.f
			};
			vertexBufferCreateInfos.vertexComponentCount = 5;
			if (m_vertexBuffer.create(vertexBufferCreateInfos) != sl::Result::eSuccess)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't crete vertex buffer");


			std::ifstream vertexFile {"shaders/test.vert.spv", std::ios::binary};
			if (!vertexFile)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't open 'shaders/test.vert.spv'");
			std::vector<std::byte> vertexSpirv {sl::utils::readBinaryFile(vertexFile)};
			vertexFile.close();

			std::ifstream fragmentFile {"shaders/test.frag.spv", std::ios::binary};
			if (!fragmentFile)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't open 'shaders/test.frag.spv'");
			std::vector<std::byte> fragmentSpirv {sl::utils::readBinaryFile(fragmentFile)};
			fragmentFile.close();


			sl::render::vulkan::ShaderCreateInfos vertexShaderCreateInfos {};
			vertexShaderCreateInfos.instance = &m_renderer.getInstance();
			vertexShaderCreateInfos.type = sl::render::vulkan::ShaderType::eVertex;
			vertexShaderCreateInfos.spirv = std::move(vertexSpirv);
			if (m_vertexShader.create(vertexShaderCreateInfos) != sl::Result::eSuccess)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't create vertex shader");

			sl::render::vulkan::ShaderCreateInfos fragmentShaderCreateInfos {};
			fragmentShaderCreateInfos.instance = &m_renderer.getInstance();
			fragmentShaderCreateInfos.type = sl::render::vulkan::ShaderType::eFragment;
			fragmentShaderCreateInfos.spirv = std::move(fragmentSpirv);
			if (m_fragmentShader.create(fragmentShaderCreateInfos) != sl::Result::eSuccess)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't create fragment shader");

			sl::render::vulkan::PipelineCreateInfos pipelineCreateInfos {};
			pipelineCreateInfos.instance = &m_renderer.getInstance();
			pipelineCreateInfos.type = sl::render::vulkan::PipelineType::eGraphics;
			pipelineCreateInfos.shaders = {&m_vertexShader, &m_fragmentShader};
			if (m_pipeline.create(pipelineCreateInfos) != sl::Result::eSuccess)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't create pipeline");

			VkCommandPoolCreateInfo commandPoolCreateInfos {};
			commandPoolCreateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfos.queueFamilyIndex = m_renderer.getInstance().getGpu()->getGraphicsQueue().familyIndex;
			commandPoolCreateInfos.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			if (vkCreateCommandPool(m_renderer.getInstance().getGpu()->getDevice(), &commandPoolCreateInfos, nullptr, &m_commandPool) != VK_SUCCESS)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't create command pool");

			VkCommandBufferAllocateInfo commandBufferAllocateInfos {};
			commandBufferAllocateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			commandBufferAllocateInfos.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			commandBufferAllocateInfos.commandPool = m_commandPool;
			commandBufferAllocateInfos.commandBufferCount = 1;
			if (vkAllocateCommandBuffers(m_renderer.getInstance().getGpu()->getDevice(), &commandBufferAllocateInfos, &m_commandBuffer) != VK_SUCCESS)
				return sl::ErrorStack::push(sl::Result::eFailure, "Can't allocate command buffers");

			VkSemaphoreCreateInfo semaphoreCreateInfos {};
			semaphoreCreateInfos.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			(void)vkCreateSemaphore(m_renderer.getInstance().getGpu()->getDevice(), &semaphoreCreateInfos, nullptr, &m_imageAvailableSemaphore);
			(void)vkCreateSemaphore(m_renderer.getInstance().getGpu()->getDevice(), &semaphoreCreateInfos, nullptr, &m_renderFinishedSemaphore);

			VkFenceCreateInfo fenceCreateInfos {};
			fenceCreateInfos.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fenceCreateInfos.flags = VK_FENCE_CREATE_SIGNALED_BIT;
			(void)vkCreateFence(m_renderer.getInstance().getGpu()->getDevice(), &fenceCreateInfos, nullptr, &m_waitForFrameFence);

			return sl::Result::eSuccess;
		}


		auto onUpdate(sl::utils::Millisecond /*dt*/) noexcept -> std::expected<bool, sl::Result> override {
			//sl::mainLogger.info("DT : {}", dt);
			if (sl::InputManager::isKeyDown(sl::Key::eEscape))
				return false;

			(void)vkWaitForFences(m_renderer.getInstance().getGpu()->getDevice(), 1, &m_waitForFrameFence, VK_TRUE, std::numeric_limits<std::uint64_t>::max());
			(void)vkResetFences(m_renderer.getInstance().getGpu()->getDevice(), 1, &m_waitForFrameFence);

			std::uint32_t imageIndex {};
			(void)vkAcquireNextImageKHR(
				m_renderer.getInstance().getGpu()->getDevice(),
				m_renderer.getSwapchain().getSwapchain(),
				std::numeric_limits<std::uint64_t>::max(),
				m_imageAvailableSemaphore,
				VK_NULL_HANDLE,
				&imageIndex
			);


			(void)vkResetCommandBuffer(m_commandBuffer, 0);

			VkCommandBufferBeginInfo beginInfos {};
			beginInfos.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfos.pInheritanceInfo = nullptr;
			if (vkBeginCommandBuffer(m_commandBuffer, &beginInfos) != VK_SUCCESS)
				return sl::ErrorStack::push(std::unexpected(sl::Result::eFailure), "Can't begin command buffer");


			VkImageMemoryBarrier imageMemoryBarrier {};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.image = m_renderer.getSwapchain().getImages()[imageIndex];
			imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageMemoryBarrier.subresourceRange.levelCount = 1;
			imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
			imageMemoryBarrier.subresourceRange.layerCount = 1;
			imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
			imageMemoryBarrier.dstQueueFamilyIndex = m_renderer.getInstance().getGpu()->getGraphicsQueue().familyIndex;

			vkCmdPipelineBarrier(
				m_commandBuffer,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier
			);

			VkRenderingAttachmentInfo renderingAttachmentInfos {};
			renderingAttachmentInfos.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
			renderingAttachmentInfos.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			renderingAttachmentInfos.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			renderingAttachmentInfos.clearValue.color.float32[0] = 0.f;
			renderingAttachmentInfos.clearValue.color.float32[1] = 0.f;
			renderingAttachmentInfos.clearValue.color.float32[2] = 0.f;
			renderingAttachmentInfos.clearValue.color.float32[3] = 0.f;
			renderingAttachmentInfos.clearValue.depthStencil.stencil = 0.f;
			renderingAttachmentInfos.clearValue.depthStencil.depth = 0.f;
			renderingAttachmentInfos.imageView = m_renderer.getSwapchain().getImageViews()[imageIndex];
			renderingAttachmentInfos.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			renderingAttachmentInfos.resolveMode = VK_RESOLVE_MODE_NONE;

			VkRenderingInfo renderingInfos {};
			renderingInfos.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
			renderingInfos.colorAttachmentCount = 1;
			renderingInfos.pColorAttachments = &renderingAttachmentInfos;
			renderingInfos.layerCount = 1;
			renderingInfos.renderArea = {{0, 0}, m_renderer.getSwapchain().getImageExtent()};
			vkCmdBeginRendering(m_commandBuffer, &renderingInfos);

			VkViewport viewport {};
			viewport.width = m_renderer.getSwapchain().getImageExtent().width;
			viewport.height = m_renderer.getSwapchain().getImageExtent().height;
			vkCmdSetViewport(m_commandBuffer, 0, 1, &viewport);

			VkRect2D scissors {};
			scissors.extent = m_renderer.getSwapchain().getImageExtent();
			vkCmdSetScissor(m_commandBuffer, 0, 1, &scissors);

			vkCmdBindPipeline(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline.getPipeline());

			VkBuffer buffers[] {m_vertexBuffer.getBuffer()};
			VkDeviceSize offsets[] {0};
			vkCmdBindVertexBuffers(m_commandBuffer, 0, 1, buffers, offsets);

			vkCmdDraw(m_commandBuffer, 3, 1, 0, 0);

			vkCmdEndRendering(m_commandBuffer);

			VkImageMemoryBarrier imageMemoryBarrier2 {};
			imageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier2.image = m_renderer.getSwapchain().getImages()[imageIndex];
			imageMemoryBarrier2.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
			imageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			imageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			imageMemoryBarrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			imageMemoryBarrier2.subresourceRange.levelCount = 1;
			imageMemoryBarrier2.subresourceRange.baseMipLevel = 0;
			imageMemoryBarrier2.subresourceRange.layerCount = 1;
			imageMemoryBarrier2.subresourceRange.baseArrayLayer = 0;

			vkCmdPipelineBarrier(
				m_commandBuffer,
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier2
			);

			if (vkEndCommandBuffer(m_commandBuffer) != VK_SUCCESS)
				return sl::ErrorStack::push(std::unexpected(sl::Result::eFailure), "Can't end command buffer");


			VkPipelineStageFlags pipelineStageFlags {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

			VkSubmitInfo submitInfos {};
			submitInfos.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfos.commandBufferCount = 1;
			submitInfos.pCommandBuffers = &m_commandBuffer;
			submitInfos.waitSemaphoreCount = 1;
			submitInfos.pWaitSemaphores = &m_imageAvailableSemaphore;
			submitInfos.pWaitDstStageMask = &pipelineStageFlags;
			submitInfos.signalSemaphoreCount = 1;
			submitInfos.pSignalSemaphores = &m_renderFinishedSemaphore;
			if (vkQueueSubmit(m_renderer.getInstance().getGpu()->getGraphicsQueue().queues[0], 1, &submitInfos, m_waitForFrameFence) != VK_SUCCESS)
				return sl::ErrorStack::push(std::unexpected(sl::Result::eFailure), "Can't submit queue");

			VkSwapchainKHR swapchains[] {m_renderer.getSwapchain().getSwapchain()};
			VkPresentInfoKHR presentInfos {};
			presentInfos.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfos.waitSemaphoreCount = 1;
			presentInfos.pWaitSemaphores = &m_renderFinishedSemaphore;
			presentInfos.swapchainCount = 1;
			presentInfos.pSwapchains = swapchains;
			presentInfos.pImageIndices = &imageIndex;
			if (vkQueuePresentKHR(m_renderer.getInstance().getGpu()->getPresentQueue().queues[0], &presentInfos) != VK_SUCCESS)
				return sl::ErrorStack::push(std::unexpected(sl::Result::eFailure), "Can't present queue");

			return false;
//			return true;
		}


		auto onDestruction() noexcept -> void override {
			std::println("Destruction");
			if (m_waitForFrameFence != VK_NULL_HANDLE)
				vkDestroyFence(m_renderer.getInstance().getGpu()->getDevice(), m_waitForFrameFence, nullptr);
			if (m_renderFinishedSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(m_renderer.getInstance().getGpu()->getDevice(), m_renderFinishedSemaphore, nullptr);
			if (m_imageAvailableSemaphore != VK_NULL_HANDLE)
				vkDestroySemaphore(m_renderer.getInstance().getGpu()->getDevice(), m_imageAvailableSemaphore, nullptr);
			if (m_commandBuffer != VK_NULL_HANDLE)
				vkFreeCommandBuffers(this->m_renderer.getInstance().getGpu()->getDevice(), this->m_commandPool, 1, &m_commandBuffer);
			if (m_commandPool != VK_NULL_HANDLE)
				vkDestroyCommandPool(m_renderer.getInstance().getGpu()->getDevice(), m_commandPool, nullptr);
			m_pipeline.destroy();
			m_fragmentShader.destroy();
			m_vertexShader.destroy();
			m_vertexBuffer.destroy();
		}

	private:
		VkCommandPool m_commandPool;
		VkCommandBuffer m_commandBuffer;
		sl::render::vulkan::Shader m_vertexShader;
		sl::render::vulkan::Shader m_fragmentShader;
		sl::render::vulkan::Pipeline m_pipeline;
		sl::render::vulkan::VertexBuffer m_vertexBuffer;
		VkSemaphore m_imageAvailableSemaphore;
		VkSemaphore m_renderFinishedSemaphore;
		VkFence m_waitForFrameFence;
};


auto createApplication() noexcept -> std::unique_ptr<sl::Application> {
	return std::make_unique<SandboxApp> ();
}
