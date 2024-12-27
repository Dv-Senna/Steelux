#define SL_ENTRY_POINT
#include <sl/Steelux.hpp>

#include <sl/utils/enums.hpp>
#include <sl/utils/endianness.hpp>
#include <sl/utils/string.hpp>

#include <ranges>
#include <memory>
#include <print>
#include <vector>



struct SBO {
	char8_t *start;
	std::size_t length;
	std::size_t capacity;
};

using namespace sl::utils::literals;

template <typename T>
class DebugAllocator {
	public:
		using value_type = T;
		DebugAllocator() : m_allocationCount {0} {}
		~DebugAllocator() {
			std::println("Debug allocator :\n\tallocation count : {}", m_allocationCount);
		}

		[[nodiscard]] T *allocate(std::size_t n) {
			++m_allocationCount;
			return reinterpret_cast<T*> (std::malloc(sizeof(T) * n));
		}

		void deallocate(T *ptr, std::size_t n) {
			std::free(ptr);
		}

	private:
		std::size_t m_allocationCount;
};


class SandboxApp final : public sl::Application {
	public:
		SandboxApp() : sl::Application() {
			std::println("c8* : {}/{}, size_t : {}/{}, SBO : {}/{}", alignof(char8_t*), sizeof(char8_t*), alignof(std::size_t), sizeof(std::size_t), alignof(SBO), sizeof(SBO));
			m_infos.name = "Sandbox";

			std::println("std::string : {}/{}, sl::utils::String : {}/{}", alignof(std::string), sizeof(std::string), alignof(sl::utils::String), sizeof(sl::utils::String));

			DebugAllocator<char> debugAllocator {};

			sl::utils::String test {"ABCDEFGHIJKLMNOPQRSTUVW", &debugAllocator};
			const sl::utils::String test2 {std::move(test)};
			sl::utils::String test3 {test2};
			std::println("test : {}", (std::size_t)test.getData());
			std::println("test2 : {}", test2.getData());
			std::println("test3 : {}", test3.getData());
			test3[5] = '_';
			std::println("test3 : {}", test3.getData());

			for (auto &c : test3) {
				++c;
			}

			std::println("test3 : {}", test3.getData());
			test3.pushBack('Z');
			test3.insert(test3.begin() + 5, '$');
			std::println("INSERT test3 : {}", test3.getData());

			test3.popFront();
			test3.erase(1, 3);
			std::println("ERASE test3 : {}", test3.getData());

			test3[-1] = '@';
			std::println("test3 : {}", test3.getData());

			test3 += test2;
			std::println("BACK_CONCAT test3 : {}", test3.getData());

			sl::utils::String test4 {test2 + " Hello " + test3 + " World !"};
			std::println("test4 : {}", test4.getData());
		}

		~SandboxApp() override {

		}

		sl::Result onCreation() noexcept override {
			std::println("Creation");
			return sl::Result::eSuccess;
		}

		void onDestruction() noexcept override {
			std::println("Destruction");
		}
};


std::unique_ptr<sl::Application> createApplication() {
	return std::make_unique<SandboxApp> ();
}
