cmake_minimum_required(VERSION 3.20)


set(TESTS_NAME ${PROJECT_NAME}_tests)
set(TESTS_VERSION ${PROJECT_VERSION})
set(TESTS_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(TESTS_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(TESTS_VERSION_PATCH ${PROJECT_VERSION_PATCH})
string(TOUPPER ${TESTS_NAME} TESTS_NAME_UPPERCASE)

file(GLOB_RECURSE TESTS_SOURCE src/*.cpp)

add_subdirectory(vendors/Catch2)

foreach(TEST_SOURCE ${TESTS_SOURCE})
	get_filename_component(TEST_FILENAME ${TEST_SOURCE} NAME_WE)
	set(TEST_NAME ${TESTS_NAME}_${TEST_FILENAME})

	add_compile_definitions(
		${TESTS_NAME_UPPERCASE}_NAME=${TESTS_NAME}_${TEST_TESTFILENAME}
		${TESTS_NAME_UPPERCASE}_VERSION_MAJOR=${TESTS_VERSION_MAJOR}
		${TESTS_NAME_UPPERCASE}_VERSION_MINOR=${TESTS_VERSION_MINOR}
		${TESTS_NAME_UPPERCASE}_VERSION_PATCH=${TESTS_VERSION_PATCH}
	)

	if (BUILD_STATIC)
		add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_STATIC)
	else()
		add_compile_definitions(${PROJECT_ACRONYM_UPPERCASE}_SHARED)
	endif()

	include_directories(
		./include,
		../lib/include
	)

	add_executable(${TEST_NAME} ${TEST_SOURCE})
	set_property(TARGET ${TEST_NAME} PROPERTY CXX_STANDARD ${PROJECT_CPP_DIALECT})

	if (MSVC)
		target_compile_options(${TEST_NAME} PRIVATE /W4)
	else()
		target_compile_options(${TEST_NAME} PRIVATE -Wall -Wextra -pedantic)
	endif()

	target_link_libraries(
		${TEST_NAME} PRIVATE ${PROJECT_NAME} Catch2::Catch2WithMain
	)
endforeach()



if (WIN32)
	add_compile_definitions(${TESTS_NAME_UPPERCASE}_WINDOWS)
	if (BUILD_STATIC)
	else()
		message("Copying dll")
		message("${PROJECT_BINARY_DIR}/lib/lib${PROJECT_NAME}.dll")
		message("${PROJECT_BINARY_DIR}/sandbox")
		add_custom_command(
			TARGET ${TESTS_NAME}
			PRE_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
			"${PROJECT_BINARY_DIR}/lib/lib${PROJECT_NAME}.dll"
			"${PROJECT_BINARY_DIR}/sandbox"
		)
	endif()
elseif (UNIX)
	add_compile_definitions(${TESTS_NAME_UPPERCASE}_LINUX)
elseif (APPLE)
	add_compile_definitions(${TESTS_NAME_UPPERCASE}_APPLE)
else()
	message(FATAL_ERROR "Platforms other than Windows, Linux and MacOS aren't supported for now")
endif()
