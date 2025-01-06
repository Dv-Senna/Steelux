cmake_minimum_required(VERSION 3.20)

project(xdg_shell LANGUAGES C VERSION 1.0.0)


set(XDG_SHELL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(XDG_SHELL_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(XDG_HEADER_FILE ${XDG_SHELL_INCLUDE_DIR}/xdg-shell/xdg-shell-client-protocol.h)
set(XDG_SOURCE_FILE ${XDG_SHELL_SOURCE_DIR}/xdg-shell-protocol.c)
set(XDG_CONFIG_FILE /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml)

# header
make_directory(${XDG_SHELL_INCLUDE_DIR}/xdg-shell)
add_custom_command(
	OUTPUT
		${XDG_HEADER_FILE}
	COMMAND
	wayland-scanner client-header ${XDG_CONFIG_FILE} ${XDG_HEADER_FILE}
)


# source
make_directory(${XDG_SHELL_SOURCE_DIR})
add_custom_command(
	OUTPUT
	${XDG_SOURCE_FILE}
	COMMAND
	wayland-scanner private-code ${XDG_CONFIG_FILE} ${XDG_SOURCE_FILE}
)


# library
add_library(xdg_shell_library STATIC ${XDG_SOURCE_FILE} ${XDG_HEADER_FILE})
set_target_properties(xdg_shell_library PROPERTIES LINKER_LANGUAGE C)
target_include_directories(xdg_shell_library PUBLIC ${XDG_SHELL_INCLUDE_DIR})

# headers (for include)
add_library(xdg_shell_headers INTERFACE)
target_include_directories(xdg_shell_headers INTERFACE ${XDG_SHELL_INCLUDE_DIR})

# alias
add_library(xdg_shell::xdg_shell ALIAS xdg_shell_library)
add_library(xdg_shell::Headers ALIAS xdg_shell_headers)
