cmake_minimum_required(VERSION 3.8)
project(app)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)




if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
  # Default setting
  message(STATUS "Target system is Linux")

  # Find dependencies
  find_package(glfw3 REQUIRED)

  set(OpenGL_GL_PREFERENCE GLVND)
  find_package(OpenGL REQUIRED)

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

  add_executable(${PROJECT_NAME}
    src/main.cpp
    src/gui.cpp
    src/imagesource.cpp
    src/aui.cpp
  )

  target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

  target_link_libraries(${PROJECT_NAME} PRIVATE
    ImGui
    glfw
    OpenGL::GL
  )

  target_compile_definitions(ImGui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)


elseif(${CMAKE_SYSTEM_NAME} MATCHES Windows)

else()
  # Default setting
endif()



