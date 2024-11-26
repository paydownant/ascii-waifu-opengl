#ifndef GUI_H
#define GUI_H

#define GL_SILENCE_DEPRECATION

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "aui.h"



class GUI {

private:

  enum WindowStyle {
    DARK,
    LIGHT
  };

  char *glsl_version = nullptr;
  int window_width, window_height;
  GLFWwindow* window = nullptr;
  char *window_title = nullptr;
  int window_style = DARK;
  int font_size = 0;

  char *aui_path;

public:
  GUI();
  ~GUI();

  void run();
  void aui_window(bool is_open, AUI aui);

};

#endif