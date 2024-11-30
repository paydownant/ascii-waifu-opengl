
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "gui.h"


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

  aui_path = strdup("../images/makima.jpg");

  ascii_font_path = strdup("../fonts/Technology/Technology.ttf");
  tool_font_path = strdup("../fonts/OpenSans/OpenSans-VariableFont_wdth,wght.ttf");

  ascii_set = strdup("LUCY");
  
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
  draw_properties.tool_font.font = io.Fonts->AddFontFromFileTTF(tool_font_path, draw_properties.tool_font.size);
  load_ascii_fonts(io.Fonts, ascii_font_path);
  
  // Draw properties
  draw_properties.aspect_ratio = 0.4f;
  draw_properties.ascii_scale = 1.0f;
  draw_properties.ascii_font.font = font.fonts[3];
  draw_properties.ascii_font.size = font.sizes[3];
  draw_properties.ascii_font.size_slider = font.sizes[3];
  update_resolution();
  

  // State
  ImVec4 clear_color = bg_colour;
  w_open_ascii = true;
  w_open_tool = true;
  
  // Initial image

  aui = new AUI();
  aui->load_base_image(aui_path);
  aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);

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

void GUI :: ascii_window(bool is_open) {
  ImGui::SetNextWindowSize(ImVec2((float)display_w, (float)display_h));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowBgAlpha(0.0);

  ImGui::Begin("Phantom", &is_open, ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);

  ImGui::Text(aui_path);

  draw_ascii();  
    
  ImGui::End();
}

void GUI :: tool_window(bool is_open) {
  ImGui::PushFont(draw_properties.tool_font.font);
  ImGui::Begin("Tools", &is_open, 0);
  
  widgets.slider_scale = ImGui::SliderFloat("Scale", &draw_properties.ascii_scale, 0.1, 5.0, "%.2f Chrs");
  widgets.slider_aspect_ratio = ImGui::SliderFloat("Aspect Ratio", &draw_properties.aspect_ratio, 0.1, 0.9, "%.2f");
  widgets.slider_font_size = ImGui::SliderInt("Font Size", &draw_properties.ascii_font.size_slider,  font.sizes.front(), font.sizes.back(), "%d px");
  
  ImGui::End();
  ImGui::PopFont();
}


void GUI :: draw_ascii() {
  draw_properties.resolution = draw_properties.ascii_scale * window_width / draw_properties.ascii_font.size;

  if (widgets.slider_scale || widgets.slider_aspect_ratio) {
    // update vertex buffer
    aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);
  }

  if (widgets.slider_font_size) {
    // update font size
    update_font_size();
    update_resolution();
    aui->createVertexBuffer(draw_properties.resolution, draw_properties.aspect_ratio);

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
  for (auto size : font.sizes) {
    if (size == draw_properties.ascii_font.size_slider) {
      draw_properties.ascii_font.size = size;
      draw_properties.ascii_font.font = font.fonts[fp];
      break;
    }
    fp++;
  }
  draw_properties.ascii_font.size_slider = draw_properties.ascii_font.size;
  
}

void GUI :: load_ascii_fonts(ImFontAtlas *font_atlas, char *font_path) {
  if (font.name != nullptr) {
    return;
  }
  font.name = font_path;
  uint i = 0;
  for (auto size : font.sizes) {
    font.fonts.push_back((*font_atlas).AddFontFromFileTTF(font_path, size));
    i++;
  }
}


static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}