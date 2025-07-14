cmake_minimum_required(VERSION 3.14)
project(opengl_imgui_game LANGUAGES CXX)

# --- Project settings ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Find system packages via pkg-config ---
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(GLEW REQUIRED glew)

# GLM is header-only; MSYS2 installs it under ucrt64/include/glm
find_path(GLM_INCLUDE_DIR glm/glm.hpp)
if (NOT GLM_INCLUDE_DIR)
  message(FATAL_ERROR "Could not find GLM headers")
endif()

# --- Include directories ---
include_directories(
  ${GLFW_INCLUDE_DIRS}
  ${GLEW_INCLUDE_DIRS}
  ${GLM_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/external/imgui
  ${CMAKE_SOURCE_DIR}/external/imgui/backends
)

# --- ImGui ---
add_library(imgui STATIC
  external/imgui/imgui.cpp
  external/imgui/imgui_draw.cpp
  external/imgui/imgui_tables.cpp
  external/imgui/imgui_widgets.cpp
  external/imgui/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC
  external/imgui
)

add_library(imgui_impl_glfw STATIC
  external/imgui/backends/imgui_impl_glfw.cpp
)
target_link_libraries(imgui_impl_glfw
  PUBLIC imgui
         ${GLFW_LIBRARIES}
)
target_include_directories(imgui_impl_glfw PUBLIC
  external/imgui
  external/imgui/backends
)

add_library(imgui_impl_opengl3 STATIC
  external/imgui/backends/imgui_impl_opengl3.cpp
)
target_link_libraries(imgui_impl_opengl3
  PUBLIC imgui
         ${GLEW_LIBRARIES}
)
target_include_directories(imgui_impl_opengl3 PUBLIC
  external/imgui
  external/imgui/backends
)

# --- Your executable ---
add_executable(MyGame
  src/main.cpp
)

target_link_libraries(MyGame
  PRIVATE
    imgui
    imgui_impl_glfw
    imgui_impl_opengl3
    ${GLFW_LIBRARIES}
    ${GLEW_LIBRARIES}
    # GLM is header-only, no link-libs
)

# Link OpenGL on Windows
if (WIN32)
  target_link_libraries(MyGame PRIVATE opengl32)
endif()

# --- Additional flags from pkg-config (optional) ---
target_compile_options(MyGame PRIVATE ${GLFW_CFLAGS_OTHER} ${GLEW_CFLAGS_OTHER})
