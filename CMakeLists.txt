set(CMAKE_PROJECT_MINIMUM_VERSION 3.20)
cmake_minimum_required(VERSION 3.20)

project(Steelux
	VERSION 0.0.0
	LANGUAGES CXX
)

set(PROJECT_ACRONYM sl)
string(TOUPPER ${PROJECT_ACRONYM} PROJECT_ACRONYM_UPPERCASE)
set(PROJECT_CPP_DIALECT 23)

option(BUILD_TESTS "BUILD_TESTS" OFF)

find_package(Vulkan REQUIRED)
#message(${Vulkan_glslc_FOUND})
#message(${Vulkan_GLSLC_EXECUTABLE})

set(DEPS_INCLUDE_DIRS
	Vulkan::Headers
)
set(DEPS_LIBRARIES
	Vulkan::Vulkan
)

add_subdirectory(lib)
add_subdirectory(sandbox)
if (SL_BUILD_TESTS)
	add_subdirectory(tests)
endif()
