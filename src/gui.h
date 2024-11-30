#ifndef GUI_H
#define GUI_H

#define GL_SILENCE_DEPRECATION

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "aui.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif


class GUI {

private:

  enum WindowStyle {
    DARK,
    LIGHT
  };

  struct DrawProperties {
    int resolution;
    float aspect_ratio;
    int font_size;
  };

  struct Widgets {
    bool slider_resolution = false;
    bool slider_aspect_ratio = false;
    bool slider_font_size = false;
  };
  
  char *glsl_version = nullptr;
  int window_width = 0, window_height = 0;
  GLFWwindow* window = nullptr;
  char *window_title = nullptr;
  int window_style = DARK;

  ImVec4 bg_colour;

  int display_w = 0, display_h = 0;

  char *aui_path = nullptr;
  AUI *aui = nullptr;

  char *ascii_font_path = nullptr;
  ImFont *ascii_font = nullptr;
  float ascii_aspect_ratio;
  float ascii_scaling;

  auchar *ascii_set = nullptr;

  DrawProperties draw_properties;
  Widgets widgets;
  
  bool w_open_ascii, w_open_tool;
  
  void ascii_window(bool is_open);
  void tool_window(bool is_open);

  void draw_ascii();

public:
  GUI();
  ~GUI();

  void run();
  

};

static void glfw_error_callback(int error, const char* description);

#endif