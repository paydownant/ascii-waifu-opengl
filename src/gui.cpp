
#include <GLFW/glfw3.h>

#include "gui.h"
#include "gui_widgets.h"
#include "gui_helper.h"

#include "font_bin_compressed.h"

GUI :: GUI() {
  glsl_version = strdup("#version 130");

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  display_w = 2000;
  display_h = 1300;

  window_title = strdup("AsciiWaifu");
  window_style = DARK;

  bg_colour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

  aui_path = strdup("image.png");

  custom_font_path = strdup("font.ttf");

  draw_properties.ascii_set = strdup("O");
  draw_properties.ascii_scale = draw_properties.default_val.ascii_scale;
  draw_properties.aspect_ratio = draw_properties.default_val.aspect_ratio;
  draw_properties.ascii_font.size_slider = font_pixels.sizes[draw_properties.default_val.font_set_index];
  draw_properties.tool_window_ratio = .18f;

  output_path = strdup("output.png");
}

GUI :: ~GUI() {
  printf("Terminating GUI\n");
  // Clean up
  delete(aui);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  clean_gui_mem();
}

void GUI :: run() {
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  window = glfwCreateWindow(display_w, display_h, window_title, nullptr, nullptr);
  if (window == nullptr) {
    return;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Setup ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Setup ImGui style
  switch (window_style) {
  case DARK:
    ImGui::StyleColorsDark();
    break;
  case LIGHT:
    ImGui::StyleColorsLight();
    break;
  default:
    ImGui::StyleColorsDark();
    break;
  }

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);

  // Load fonts here
  font_atlas = io.Fonts;
  ImFontConfig font_config;

  load_fonts();

  // State
  ImVec4 clear_color = bg_colour;

  // Initiate AUI
  aui = new AUI();

  //aui->load_base_image(aui_path);
  //aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);

  update_resolution();

  // Main Loop
  while (!glfwWindowShouldClose(window)) {
    
    // Poll and handle events
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Body

    ascii_window();
    
    tool_window();

    bounds_window();

    // Body End

    // Rendering
    ImGui::Render();

    // Process Input
    process_input();
    
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
  }
}

void GUI :: process_input() {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  bool pend_update_buffer = false;

  if (widgets.button_load_base_image) {
    pend_update_buffer = true;
    aui->load_base_image(aui_path);
  }

  if (widgets.slider_scale || widgets.slider_aspect_ratio) {
    // update vertex buffer
    pend_update_buffer = true;
  }

  if (widgets.button_reset_scale) {
    pend_update_buffer = true;
    draw_properties.ascii_scale = draw_properties.default_val.ascii_scale;
  }

  if (widgets.button_reset_aspect_ratio) {
    pend_update_buffer = true;
    draw_properties.aspect_ratio = draw_properties.default_val.aspect_ratio;
  }

  if (widgets.slider_font_size) {
    // update font size
    pend_update_buffer = true;
    update_font_size();
  }

  if (widgets.button_reset_font_size) {
    pend_update_buffer = true;
    draw_properties.ascii_font.size_slider = font_pixels.sizes[draw_properties.default_val.font_set_index];
    update_font_size();
  }

  if (widgets.input_ascii_char) {
    // no need to implement for now since ascii_set is updated every frame
    pend_update_buffer = true;
  }

  if (widgets.button_load_custom_font) {
    // update font
    pend_update_buffer = true;
    load_fonts();
    update_font_size();
  }
  
  // GUI update should be placed above
  if (pend_update_buffer) {
    if (aui->is_base_img_loaded()) {
      update_resolution();
      aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);
    }
  }

  if (widgets.button_export_img) {
    export_img();
  }
}

void GUI :: ascii_window() {
  ImGuiWindowFlags flags = 
  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar  |
  ImGuiWindowFlags_NoScrollbar        | ImGuiWindowFlags_NoDecoration          | ImGuiWindowFlags_NoResize    |
  ImGuiWindowFlags_NoMove             | ImGuiWindowFlags_NoSavedSettings       | ImGuiWindowFlags_NoNav;
  ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowBgAlpha(0.0);

  ImGui::Begin("Render", NULL, flags);

  draw_ascii();
    
  ImGui::End();
}

void GUI :: tool_window() {
  ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings;

  ImGui::SetNextWindowPos(ImVec2(display_w - display_w * draw_properties.tool_window_ratio, 0));
  ImGui::SetNextWindowSize(ImVec2(display_w * draw_properties.tool_window_ratio, display_h));
  ImGui::SetNextWindowBgAlpha(0.8);

  ImGui::PushFont(draw_properties.window_font.font);
  ImGui::Begin("Tools", NULL, flags);
  
  widgets.button_load_base_image = gui_path_load_button(*this, "Image Path", &aui_path);
  
  widgets.slider_scale = gui_slider_float(*this, "Scale", &draw_properties.ascii_scale, 0.10, 3.00);
  widgets.button_reset_scale = gui_reset_button_sameline(*this, "Scale");

  widgets.slider_aspect_ratio = gui_slider_float(*this, "Aspect Ratio", &draw_properties.aspect_ratio, 0.01, 0.99);
  widgets.button_reset_aspect_ratio = gui_reset_button_sameline(*this, "Aspect Ratio");

  widgets.slider_font_size = gui_slider_int(*this, "Font Size", &draw_properties.ascii_font.size_slider, font_pixels.sizes.front(), font_pixels.sizes.back());
  widgets.button_reset_font_size = gui_reset_button_sameline(*this, "Font Size");

  widgets.input_ascii_char = gui_text_input(*this, "Ascii Set", &draw_properties.ascii_set);
  
  widgets.button_load_custom_font = gui_path_load_button(*this, "Custom Font Path", &custom_font_path);

  widgets.button_export_img = gui_export_img_button(*this, "Export Window as Image", &output_path);

  ImGui::End();
  ImGui::PopFont();
}

void GUI :: bounds_window() {
  if (!widgets.shape_bounds) {
    return;
  }

  ImGuiWindowFlags flags = 
  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar      |
  ImGuiWindowFlags_NoResize           | ImGuiWindowFlags_NoMove                | ImGuiWindowFlags_NoSavedSettings | 
  ImGuiWindowFlags_NoScrollbar        | ImGuiWindowFlags_NoDecoration          | ImGuiWindowFlags_NoNav;

  unsigned int boundary_size_w = display_w * (1 - draw_properties.tool_window_ratio) - draw_properties.boundary.x_min;
  unsigned int boundary_size_h = display_h - draw_properties.boundary.y_min;

  ImGui::SetNextWindowPos(ImVec2(draw_properties.boundary.x_min, draw_properties.boundary.y_min));
  ImGui::SetNextWindowSize(ImVec2(boundary_size_w, boundary_size_h));
  ImGui::SetNextWindowBgAlpha(0.0);

  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
  ImGui::Begin("Bounds", NULL, flags);
  ImGui::End();
  ImGui::PopStyleColor();

}

void GUI :: draw_ascii() {
  if (!aui->is_base_img_loaded()) {
    return;
  }

  ascii_data_t* data = aui->getAsciiBuffer(draw_properties.ascii_set, (unsigned int)strlen(draw_properties.ascii_set));
  if (data == nullptr) {
    return;
  }
  ImGui::PushFont(draw_properties.ascii_font.font);
  ImVec2 tex_size = ImGui::CalcTextSize("O");
  float font_w = tex_size.x, font_h = tex_size.y;
  for (unsigned int y = 0; y < data->height; ++y) {
    for (unsigned int x = 0; x < data->width; ++x) {
      float r_level, g_level, b_level;
      r_level = data->colour_strip[y * data->width + x].x;
      g_level = data->colour_strip[y * data->width + x].y;
      b_level = data->colour_strip[y * data->width + x].z;
      
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r_level, g_level, b_level, 1.0f));
      ImGui::SetCursorPosX(font_w * x);
      ImGui::SetCursorPosY(font_h * y);
      ImGui::Text("%c", data->char_strip[y * data->width + x]);
      
      ImGui::PopStyleColor();

      ImGui::SameLine(0.0f, 0.0f);
    }
    ImGui::NewLine();
  }
  ImGui::PopFont();
  
  // Calculating Drawing Boundary
  draw_properties.boundary.x_min = 0;
  draw_properties.boundary.y_min = 0;

  draw_properties.boundary.x_max = font_w * data->width + draw_properties.boundary.x_min;
  draw_properties.boundary.y_max = font_h * data->height + draw_properties.boundary.y_min;
  
  free(data->char_strip);
  free(data->colour_strip);
  free(data);
  
  unsigned int drawable_max_x = display_w * (1 - draw_properties.tool_window_ratio);
  unsigned int drawable_max_y = display_h;
  // Check for Out of Bounds
  if (draw_properties.boundary.x_max > drawable_max_x || draw_properties.boundary.y_max > drawable_max_y) {
    widgets.shape_bounds = true;
  } else {
    widgets.shape_bounds = false;
  }

}

void GUI :: update_resolution() {
  draw_properties.resolution = (1 - draw_properties.tool_window_ratio) * draw_properties.ascii_scale
   * (display_w / draw_properties.ascii_font.size + ((display_w / draw_properties.ascii_font.size) * (.5f - draw_properties.aspect_ratio)));

}

void GUI :: update_font_size() {
  int fp = 0;
  for (auto size : font_pixels.sizes) {
    if (size == draw_properties.ascii_font.size_slider) {
      draw_properties.ascii_font.size = size;
      draw_properties.ascii_font.font = font_pixels.fonts[fp];
      break;
    }
    fp++;
  }
  draw_properties.ascii_font.size_slider = draw_properties.ascii_font.size;
  
}

void GUI :: load_window_font() {
  draw_properties.window_font.font = font_atlas->AddFontFromMemoryCompressedTTF(opensans_compressed_data, opensans_compressed_size, draw_properties.window_font.size, font_config, font_atlas->GetGlyphRangesDefault());
}

void GUI :: load_ascii_fonts() {
  unsigned int i = 0;
  for (auto size : font_pixels.sizes) {
    ImFont *new_ascii_font = font_atlas->AddFontFromMemoryCompressedTTF(unifont_compressed_data, unifont_compressed_size, size, font_config, font_atlas->GetGlyphRangesDefault());
    new_ascii_font->FontSize = size;
    font_pixels.fonts.push_back(new_ascii_font);
    i++;
  }

  if (font_pixels.fonts.size() != font_pixels.sizes.size()) {
    fprintf(stderr, "Font Load Error: Unmatched Font Number\n");
    return;
  }
}

void GUI :: load_custom_ascii_fonts() {
  unsigned int i = 0;
  for (auto size : font_pixels.sizes) {
    ImFont *new_ascii_font = font_atlas->AddFontFromFileTTF(custom_font_path, size, font_config, font_atlas->GetGlyphRangesDefault());
    new_ascii_font->FontSize = size;
    font_pixels.fonts.push_back(new_ascii_font);
    i++;
  }

  if (font_pixels.fonts.size() != font_pixels.sizes.size()) {
    fprintf(stderr, "Font Load Error: Unmatched Font Number\n");
    return;
  }
  draw_properties.font_loaded = true;
}

void GUI :: load_fonts() {
  if (!draw_properties.font_loaded) {
    load_window_font();
    load_ascii_fonts();
    draw_properties.ascii_font.font = font_pixels.fonts[draw_properties.default_val.font_set_index];
    draw_properties.ascii_font.size = font_pixels.sizes[draw_properties.default_val.font_set_index];
    draw_properties.ascii_font.size_slider = font_pixels.sizes[draw_properties.default_val.font_set_index];
    draw_properties.font_loaded = true;
  } else {
    if (is_file_ttf(custom_font_path)) {
      font_pixels.fonts.clear();
      font_atlas->Clear();
      load_window_font();
      load_custom_ascii_fonts();
    } else {
      return;
    }
  }

  font_atlas->Build();
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();
}

void GUI :: export_img() {

  if (widgets.shape_bounds) {
    // Out of Bounds
    return;
  }

  unsigned int channels = 4;
  unsigned int image_size_w = draw_properties.boundary.x_max - draw_properties.boundary.x_min;
  unsigned int image_size_h = draw_properties.boundary.y_max - draw_properties.boundary.y_min;

  unsigned int *buffer = new unsigned int[image_size_w * image_size_h * channels];
  if (buffer == nullptr) {
    fprintf(stderr, "Failed to Create Export Buffer\n");
    return;
  }

  glReadPixels(draw_properties.boundary.x_min, display_h - draw_properties.boundary.y_max, image_size_w, image_size_h, GL_RGBA, GL_UNSIGNED_INT, buffer);
  
  if (!export_buffer_to_img(image_size_w, image_size_h, channels, buffer, output_path)) {
    fprintf(stderr, "Failed to Export\n");
  }
  delete(buffer);
}

void GUI :: clean_gui_mem() {
  // draw_properties
  free(draw_properties.ascii_set);

  // gui stuff
  free(aui_path);
  free(custom_font_path);
  free(output_path);
  free(window_title);
  free(glsl_version);

  // font pixels stuff
  font_pixels.fonts.clear();

  // font atlas
  font_atlas->Clear();
}