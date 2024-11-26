C++ Gui Test using ImGui

Build dir is included just for the sake of example. In applications, build dir needs to be cleaned and restarted using command:
  - cmake . -B <build_dir_path>
  - or enter build dir and cmake ..

Cmake works for only for linux at this time since some dependencies fails to connect on mingw environment in linux.

Dependencies: For opengl & opengl example
  - glfw3 (apt install libglfw3 and libglfw3-dev)
  - xorg (apt install xorg-dev)
