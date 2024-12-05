cmake_minimum_required(VERSION 3.10)
project(asciiwaifu)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME}
  src/main.cpp
  src/gui.cpp
  src/imagesource.cpp
  src/aui.cpp
)

# Set where the ImGui files are stored
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/imgui)
set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

# Compile as static library
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)

add_library(ImGui STATIC
  ${IMGUI_SOURCES}
  ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
  ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)
target_include_directories(ImGui PUBLIC
  ${IMGUI_DIR}
  ${IMGUI_BACKENDS_DIR}
)
target_link_libraries(${PROJECT_NAME} PRIVATE
  ImGui
)

# Find dependencies
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
target_link_libraries(ImGui PRIVATE glfw)

 # set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)



target_include_directories(${PROJECT_NAME} PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)




