
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "gui.h"
#include "gui_widgets.h"

GUI :: GUI() {
  glsl_version = strdup("#version 130");

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window_width = 2000;
  window_height = 1300;

  window_title = strdup("AsciiWaifu");
  window_style = DARK;
  bg_colour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

  aui_path = strdup("../images/");

  ascii_font_path = strdup("../fonts/Technology/Technology.ttf");
  tool_font_path = strdup("../fonts/OpenSans/OpenSans-VariableFont_wdth,wght.ttf");

  ascii_set = strdup("O");
  
  draw_properties.aspect_ratio = 0.4f;

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

  free(ascii_set);
  free(ascii_font_path);
  free(tool_font_path);
  free(aui_path);
  free(window_title);
  free(glsl_version);
  
}

void GUI :: run() {
  
  window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
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

  load_tool_font();
  load_ascii_fonts();
  
  // Draw Properties Initial
  draw_properties.aspect_ratio = 0.4f;
  draw_properties.ascii_scale = 2.0f;

  update_resolution();
  

  // State
  ImVec4 clear_color = bg_colour;
  w_open_ascii = true;
  w_open_tool = true;
  
  // Initial image

  aui = new AUI();
  //aui->load_base_image(aui_path);
  //aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);

  // Main Loop
  while (!glfwWindowShouldClose(window)) {
    
    // Poll and handle events
    glfwPollEvents();
    if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0) {
      ImGui_ImplGlfw_Sleep(10);
      continue;
    }

    // Process Input
    process_input();

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // Body
    ascii_window(&w_open_ascii);
    tool_window(&w_open_tool);

    // Body End


    // Rendering
    ImGui::Render();
    
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
    aui->load_base_image(aui_path);
    pend_update_buffer = true;
  }

  if (widgets.slider_scale || widgets.slider_aspect_ratio) {
    // update vertex buffer
    pend_update_buffer = true;
  }

  if (widgets.slider_font_size) {
    // update font size
    pend_update_buffer = true;
    update_font_size();
  }

  if (widgets.input_ascii_char) {
    // no need to implement for now since ascii_set is updated every frame
  }

  if (widgets.button_load_ascii_font) {
    // update font
    pend_update_buffer = true;
    //load_ascii_fonts();
    update_font_size();
  }
  
  if (pend_update_buffer) {
    update_resolution();
    aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);
  }
}

void GUI :: ascii_window(bool is_open) {
  ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowBgAlpha(0.0);

  ImGui::Begin("Phantom", &is_open, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

  ImGui::PushFont(draw_properties.tool_font.font);
  ImGui::Text(aui_path);
  ImGui::PopFont();

  draw_ascii();
    
  ImGui::End();
}

void GUI :: tool_window(bool is_open) {
  ImGui::PushFont(draw_properties.tool_font.font);
  ImGui::Begin("Tools", &is_open, 0);
  
  widgets.button_load_base_image = gui_path_load_button(*this, "Image Path", &aui_path);
  widgets.slider_scale = gui_slider_float(*this, "Scale", &draw_properties.ascii_scale, 0.1, 5.0);
  widgets.slider_aspect_ratio = gui_slider_float(*this, "Aspect Ratio", &draw_properties.aspect_ratio, 0.1, 0.9);
  widgets.slider_font_size = gui_slider_int(*this, "Font Size", &draw_properties.ascii_font.size_slider, font_pixels.sizes.front(), font_pixels.sizes.back());
  widgets.input_ascii_char = gui_text_input(*this, "Ascii Set", &ascii_set);
  widgets.button_load_ascii_font = gui_path_load_button(*this, "Font Path", &ascii_font_path);

  ImGui::End();
  ImGui::PopFont();
}

void GUI :: draw_ascii() {

  if (!aui->is_base_img_loaded()) {
    return;
  }  

  ascii_data_t* data = aui->getAsciiBuffer(ascii_set, strlen(ascii_set));
  for (auint i = 0; i < data->height; ++i) {
    for (auint j = 0; j < data->width; ++j) {
      float r_level, g_level, b_level;
      r_level = data->colour_strip[i * data->width + j].x;
      g_level = data->colour_strip[i * data->width + j].y;
      b_level = data->colour_strip[i * data->width + j].z;
      
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r_level, g_level, b_level, 1.0f));
      ImGui::PushFont(draw_properties.ascii_font.font);
      ImGui::Text("%c", data->char_strip[i * data->width + j]);
      ImGui::PopFont();
      ImGui::PopStyleColor();

      ImGui::SameLine(0.0f, 0.0f);
    }
    ImGui::NewLine();
  }
  free(data->char_strip);
  free(data->colour_strip);
  free(data);
  
}

void GUI :: update_resolution() {
  draw_properties.resolution = draw_properties.ascii_scale * window_width / draw_properties.ascii_font.size;
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

void GUI :: load_tool_font() {
  draw_properties.tool_font.font = font_atlas->AddFontFromFileTTF(tool_font_path, draw_properties.tool_font.size, font_config, font_atlas->GetGlyphRangesDefault());
  font_atlas->Build();
}

void GUI :: load_ascii_fonts() {

  if (font_pixels.name != nullptr) {
    //remove_ascii_fonts();
    font_pixels.fonts.clear();
  }

  font_pixels.name = ascii_font_path;

  uint i = 0;
  for (auto size : font_pixels.sizes) {
    font_pixels.fonts.push_back(font_atlas->AddFontFromFileTTF(ascii_font_path, size, font_config, font_atlas->GetGlyphRangesDefault()));
    i++;
  }
  font_atlas->Build();

  if (font_pixels.fonts.size() != font_pixels.sizes.size()) {
    fprintf(stderr, "Font Load Error: Unmatched Font Number\n");
    return;
  }

  draw_properties.ascii_font.font = font_pixels.fonts[3];
  draw_properties.ascii_font.size = font_pixels.sizes[3];
  draw_properties.ascii_font.size_slider = font_pixels.sizes[3];
}

void GUI :: remove_ascii_fonts() {
  font_atlas->Clear();
  font_atlas->AddFontDefault();
  load_tool_font();
}


static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}