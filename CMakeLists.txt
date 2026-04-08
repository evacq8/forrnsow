cmake_minimum_required(VERSION 3.10)
project(forrnsow LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(X11 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(Threads REQUIRED)

include_directories(
	include
	extern/clap/include
	extern/clap-helpers/include
	extern/imgui
	extern/imgui/backends
)

# get all .cpp files in src
file(GLOB SRC_FILES CONFIGURE_DEPENDS "src/*.cpp")

# source files required by imgui
set(IMGUI_FILES
	extern/imgui/imgui.cpp
	extern/imgui/imgui_draw.cpp
	extern/imgui/imgui_widgets.cpp
	extern/imgui/imgui_tables.cpp
	extern/imgui/imgui_demo.cpp
	extern/imgui/backends/imgui_impl_opengl3.cpp
)

add_library(forrnsow SHARED ${SRC_FILES} ${IMGUI_FILES})

target_link_libraries(forrnsow PRIVATE
	X11::X11
	OpenGL::GL
	Threads::Threads
)

set_target_properties(forrnsow PROPERTIES
	PREFIX ""
	SUFFIX ".clap"
)

set(CLAP_INSTALL_PATH "$ENV{HOME}/.clap")
install(TARGETS forrnsow DESTINATION "${CLAP_INSTALL_PATH}")
add_custom_command(TARGET forrnsow POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory "${CLAP_INSTALL_PATH}"
	COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:forrnsow>" "${CLAP_INSTALL_PATH}/forrnsow.clap"
	COMMENT "Auto-installing to ${CLAP_INSTALL_PATH}"
)
