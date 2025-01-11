#include "sl/utils/file.hpp"


namespace sl::utils {
	auto readBinaryFile(std::ifstream &stream) noexcept -> std::vector<std::byte> {
		stream.seekg(0, std::ios::end);
		const std::size_t fileSize {static_cast<std::size_t> (stream.tellg())};
		stream.seekg(0, std::ios::beg);

		std::vector<std::byte> result {};
		result.resize(fileSize);
		stream.read(reinterpret_cast<char*> (result.data()), fileSize);

		return result;
	}

} // namespace sl::utils
