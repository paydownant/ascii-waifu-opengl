
#include <stdio.h>
#include <GLFW/glfw3.h>

#include "gui.h"


GUI :: GUI() {
  glsl_version = strdup("#version 130");
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  window_width = 1300;
  window_height = 1300;
  window_title = strdup("AsciiWaifu");
  window_style = DARK;

  aui_path = strdup("../images/satania1.png");

  window = glfwCreateWindow(window_width, window_height, window_title, nullptr, nullptr);
  if (window == nullptr) {
    return;
  }
}

GUI :: ~GUI() {

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

  // Load fonts here
  font_size = 8;

  // State
  ImVec4 clear_color = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);  // I know, I know, but leave the .f for future changes
  bool is_open;

  AUI aui;
  int ascii_resolution = window_width / font_size;
  aui.createVertexBuffer(aui_path, ascii_resolution, 0.45);
  
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
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

  }

}


void GUI :: aui_window(bool is_open, AUI aui) {
    ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
    ImGui::SetNextWindowPos(ImVec2(0, 0));

    ImGui::Begin("Phantom", &is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
    
    ImGui::Text(aui_path);
    
    ascii_data_t* data = aui.getAsciiBuffer();
    for (auint i = 0; i < data->height; ++i) {
      ImGui::Text(data->data[i]);
      free(data->data[i]);
    }
    free(data->data);
    free(data);
    
    ImGui::End();
}