cmake_minimum_required(VERSION 3.8)
project(app)

# Set the name of the target operating system
set(CMAKE_SYSTEM_NAME Linux)

if(${CMAKE_SYSTEM_NAME} MATCHES Linux)
  # Default setting
  message(STATUS "Target system is Linux")
  
elseif(${CMAKE_SYSTEM_NAME} MATCHES Windows)
  # Windows
  message(STATUS "Target system is Windows")
  # Set which compilers to use for C and C++
  set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
  set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
  # Set where the target environment located
  set(CMAKE_FIND_ROOT_PATH  /usr/x86_64-w64-mingw32)
  # adjust the default behavior of the FIND_XXX() commands:
  # search programs in the host environment
  set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
  # search headers and libraries in the target environment
  set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
  set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

else()
  # Default setting
endif()


# Find dependencies
find_package(glfw3 REQUIRED)

set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)

# Set where the ImGui files are stored
set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR}/imgui)

set(IMGUI_BACKENDS_DIR ${IMGUI_DIR}/backends)

# Compile as static library
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/*.cpp)
add_library(ImGui
  STATIC
    ${IMGUI_SOURCES}

    ${IMGUI_BACKENDS_DIR}/imgui_impl_glfw.cpp
    ${IMGUI_BACKENDS_DIR}/imgui_impl_opengl3.cpp
)
target_include_directories(ImGui PUBLIC ${IMGUI_DIR})
target_include_directories(ImGui PUBLIC ${IMGUI_BACKENDS_DIR})


add_executable(${PROJECT_NAME}
  src/main.cpp
  src/gui.cpp
  src/imagesource.cpp
  src/aui.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    ImGui
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    ImGui
    glfw
    OpenGL::GL
)

