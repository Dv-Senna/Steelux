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
option(SL_NO_SDL "NO_SDL" OFF)

find_package(Vulkan REQUIRED)
#message(${Vulkan_glslc_FOUND})
#message(${Vulkan_GLSLC_EXECUTABLE})


set(VENDORS_DIR ${CMAKE_SOURCE_DIR}/vendors)

set(DEPS_INCLUDE_DIRS
	Vulkan::Headers
	${VENDORS_DIR}/turbolin/include
)
set(DEPS_LIBRARIES
	Vulkan::Vulkan
)

if (NOT SL_NO_SDL)
	add_subdirectory(vendors/SDL)
	set(DEPS_LIBRARIES ${DEPS_LIBRARIES} SDL3::SDL3)
else()
	add_compile_definition(SL_NO_SDL)
endif()

if (UNIX)
	option(SL_NO_WAYLAND "NO WAYLAND" OFF)

	if (NOT SL_NO_WAYLAND)
		add_subdirectory(vendors/xdg-shell)
		set(DEPS_INCLUDE_DIRS ${DEPS_INCLUDE_DIRS} xdg_shell::Headers)
		set(DEPS_LIBRARIES ${DEPS_LIBRARIES} wayland-client xdg_shell::xdg_shell)
	else()
		add_compile_definition(SL_NO_WAYLAND)
	endif()
else()
	add_compile_definition(SL_NO_WAYLAND)
endif()


add_subdirectory(lib)
add_subdirectory(sandbox)
add_subdirectory(shaders)
if (SL_BUILD_TESTS)
	add_subdirectory(tests)
endif()

