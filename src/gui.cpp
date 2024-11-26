
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
  font_size = 20;

  aui_path = strdup("../images/lucy.png");

  ascii_font_path = strdup("../fonts/Technology/Technology-Bold.ttf");
  ascii_aspect_ratio = 0.4f;
  ascii_scaling = 2.2f;

  ascii_set = strdup("LUCY");

}

GUI :: ~GUI() {
  printf("Terminating GUI\n");

  free(glsl_version);
  free(window_title);
  free(aui_path);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();
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
  ascii_font = io.Fonts->AddFontFromFileTTF(ascii_font_path, font_size, NULL, io.Fonts->GetGlyphRangesDefault());

  // State
  ImVec4 clear_color = bg_colour;
  bool is_open;

  AUI *aui = new AUI();
  int ascii_resolution = ascii_scaling * window_width / font_size;
  aui->createVertexBuffer(aui_path, ascii_resolution, ascii_aspect_ratio);
  
  // Main Loop
  while (!glfwWindowShouldClose(window)) {
    // Poll and handle events
    glfwPollEvents();

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();


    // Body
    aui_window(&is_open, aui);

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


void GUI :: aui_window(bool is_open, AUI *aui) {
  ImGui::SetNextWindowSize(ImVec2(display_w, display_h));
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowBgAlpha(0.0);

  ImGui::Begin("Phantom", &is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
    
  ImGui::Text(aui_path);

  draw_ascii(aui);  
    
  ImGui::End();
}


void GUI :: draw_ascii(AUI *aui) {

  ascii_data_t* data = aui->getAsciiBuffer(ascii_set, strlen(ascii_set));
  for (auint i = 0; i < data->height; ++i) {
    for (auint j = 0; j < data->width; ++j) {
      float r_level, g_level, b_level;
      r_level = data->colour_strip[i * data->width + j].x;
      g_level = data->colour_strip[i * data->width + j].y;
      b_level = data->colour_strip[i * data->width + j].z;
      
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r_level, g_level, b_level, 1.0f));
      ImGui::PushFont(ascii_font);
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


static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}