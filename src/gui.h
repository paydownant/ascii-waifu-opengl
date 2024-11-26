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

  char *glsl_version = nullptr;
  int window_width, window_height;
  GLFWwindow* window = nullptr;
  char *window_title = nullptr;
  int window_style = DARK;

  int display_w, display_h;

  int font_size = 0;

  char *aui_path;

  void aui_window(bool is_open, AUI *aui);
  void draw_ascii(AUI *aui);

public:
  GUI();
  ~GUI();

  void run();
  

};

static void glfw_error_callback(int error, const char* description);

#endif