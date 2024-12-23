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

  struct Widgets {
    ImVec2 pos;
    ImVec2 size;
    int padding = 1;
    float ratio = 0.6;
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

  struct Notifications {
    ImVec2 pos;
    ImVec2 size;
    int padding = 5;
    int max_visible = 10;
    std::vector<std::string> pool;
  };

  struct FontPixels {
    std::vector<ImFont *> fonts;
    std::vector<int> sizes = {6,8,10,12,14,16,18,20,22,24,26,28,30,32,34,36,38,40};
  };

  struct UIStyle {
    ImVec4 col_background = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float alpha_tool_background = 0.65f;

    ImVec4 col_border = ImVec4(0.3f, 0.3f, 0.3f, 1.0f);

    ImVec4 col_widget = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    ImVec4 col_widget_hover = ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
    ImVec4 col_widget_active = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
    
    ImVec4 col_slider = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
    ImVec4 col_slider_active = ImVec4(0.6f, 0.0f, 0.0f, 1.0f);
  };

  struct UI {
    int platform = 0;
    int window_w = 0, window_h = 0;
    GLFWwindow* window = nullptr;
    std::string window_title;
    int window_style = DARK;
    UIStyle style;

    AUI *ascii_engine = nullptr;
    char *image_path = nullptr;
    char *output_path = nullptr;

    unsigned int resolution = 0;
    float aspect_ratio = 0.0;
    float ascii_scale = 0.0;

    WindowFont window_font;
    auchar *ascii_set = nullptr;
    AsciiFont ascii_font;

    FontPixels font_pixels;
    ImFontAtlas *font_atlas = nullptr;
    ImFontConfig *font_config = nullptr;
    bool font_loaded = false;
    char *custom_font_path = nullptr;
    
    Default default_val;
    
    DrawBoundary boundary;
    
    int tool_window_size = 0;
    Widgets widgets;
    Notifications notifications;
  };
  
  std::string glsl_version;
  UI ui;

  void set_window_icon();
  void process_input();

  void ascii_window();
  void tool_window();

  void draw_widgets();
  void draw_notifications();
  void draw_ascii();
  void draw_bounds();

  void update_resolution();
  void update_font_size();

  void load_window_font();
  void load_ascii_fonts();
  void load_custom_ascii_fonts();
  void load_fonts();
  void export_img();

  void push_styles();
  void pop_styles();

  void clean_gui_mem();
  

public:
  GUI();
  ~GUI();

  void run();

};

static void glfw_error_callback(int error, const char* description);

#endif