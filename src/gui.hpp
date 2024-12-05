#ifndef GUI_HPP
#define GUI_HPP

#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "aui.hpp"

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

  struct WindowFont {
    ImFont *font = nullptr;
    int size = 24;
  };

  struct AsciiFont {
    ImFont *font = nullptr;
    int size = 20;
    int size_slider = 0;
  };

  struct Default {
    const float ascii_scale = 1.0f;
    const float aspect_ratio = 0.5f;
    const int font_set_index = 7;
  };

  struct DrawBoundary {
    unsigned int x_min = 0;
    unsigned int x_max = 0;
    unsigned int y_min = 0;
    unsigned int y_max = 0;
  };

  struct DrawProperties {
    unsigned int resolution = 0;
    float aspect_ratio = 0.0;
    float ascii_scale = 0.0;
    WindowFont window_font;
    auchar *ascii_set = nullptr;
    AsciiFont ascii_font;
    bool font_loaded = false;
    Default default_val;
    DrawBoundary boundary;
    int tool_window_size = 0;
  };

  struct Widgets {
    bool button_load_base_image = false;
    bool slider_scale = false;
    bool button_reset_scale = false;
    bool slider_aspect_ratio = false;
    bool button_reset_aspect_ratio = false;
    bool slider_font_size = false;
    bool button_reset_font_size = false;
    bool input_ascii_char = false;
    bool button_load_custom_font = false;
    bool button_export_img = false;

    bool shape_bounds = false;
  };

  struct FontPixels {
    std::vector<ImFont *> fonts;
    std::vector<int> sizes = {6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40};
  };
  
  char *glsl_version = nullptr;
  int window_w = 0, window_h = 0;
  GLFWwindow* window = nullptr;
  char *window_title = nullptr;
  int window_style = DARK;

  ImVec4 bg_colour;

  char *aui_path = nullptr;
  AUI *aui = nullptr;

  char *output_path = nullptr;

  char *custom_font_path = nullptr;

  DrawProperties draw_properties;
  Widgets widgets;
  FontPixels font_pixels;

  ImFontAtlas *font_atlas = nullptr;
  ImFontConfig *font_config = nullptr;

  void process_input();

  void ascii_window();
  void tool_window();
  void bounds_window();

  void draw_ascii();
  void update_resolution();
  void update_font_size();

  void load_window_font();
  void load_ascii_fonts();
  void load_custom_ascii_fonts();
  void load_fonts();
  void refresh_fonts();
  void export_img();

  void clean_gui_mem();
  

public:
  GUI();
  ~GUI();

  void run();

};

static void glfw_error_callback(int error, const char* description);

#endif