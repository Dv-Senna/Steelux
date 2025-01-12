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

			return sl::Result::eSuccess;
		}


		auto onUpdate(sl::utils::Millisecond /*dt*/) noexcept -> std::expected<bool, sl::Result> override {
			//sl::mainLogger.info("DT : {}", dt);
			if (sl::InputManager::isKeyDown(sl::Key::eEscape))
				return false;
			return true;
		}


		auto onDestruction() noexcept -> void override {
			std::println("Destruction");
			m_pipeline.destroy();
			m_fragmentShader.destroy();
			m_vertexShader.destroy();
		}

	private:
		sl::render::vulkan::Shader m_vertexShader;
		sl::render::vulkan::Shader m_fragmentShader;
		sl::render::vulkan::Pipeline m_pipeline;
};


auto createApplication() noexcept -> std::unique_ptr<sl::Application> {
	return std::make_unique<SandboxApp> ();
}
