#ifndef GUI_H
#define GUI_H

#define GL_SILENCE_DEPRECATION

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "aui.h"

#include <vector>

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

  struct AsciiFont {
    ImFont *font = nullptr;
    int size = 20;
    int size_slider;
  };

  struct ToolFont {
    ImFont *font = nullptr;
    int size = 25;
  };

  struct DrawProperties {
    int resolution;
    float aspect_ratio;
    float ascii_scale;
    bool use_custom_font = false;
    ImFont *im_default_font = nullptr;
    AsciiFont ascii_font;
    ToolFont tool_font;
    int default_font_set_index = 7;
  };

  struct Widgets {
    bool button_load_base_image = false;
    bool slider_scale = false;
    bool slider_aspect_ratio = false;
    bool slider_font_size = false;
    bool input_ascii_char = false;
    bool button_load_ascii_font = false;
  };

  struct FontPixels {
    char *name = nullptr;
    std::vector<ImFont *> fonts;
    std::vector<int> sizes = {6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40};
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
  char *tool_font_path = nullptr;

  auchar *ascii_set = nullptr;

  DrawProperties draw_properties;
  Widgets widgets;
  FontPixels font_pixels;

  ImFontAtlas *font_atlas = nullptr;
  ImFontConfig *font_config = nullptr;
  
  bool w_open_ascii, w_open_tool;

  void process_input();
  void ascii_window(bool is_open);
  void tool_window(bool is_open);

  void draw_ascii();
  void update_resolution();
  void update_font_size();

  void load_tool_font();
  void load_ascii_fonts();
  void load_fonts();
  void remove_ascii_fonts();
  

public:
  GUI();
  ~GUI();

  void run();
  

};

static void glfw_error_callback(int error, const char* description);

#endif