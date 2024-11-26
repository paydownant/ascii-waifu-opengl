
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

  aui_path = strdup("../images/lucy.png");

  
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
  font_size = 7;

  // State
  ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);  // I know, I know, but leave the .f for future changes
  bool is_open;

  AUI *aui = new AUI();
  int ascii_resolution = window_width / font_size;
  aui->createVertexBuffer(aui_path, ascii_resolution, 0.45);
  
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
    glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);

  }

}


void GUI :: aui_window(bool is_open, AUI *aui) {
  ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
  ImGui::SetNextWindowPos(ImVec2(0, 0));

  ImGui::Begin("Phantom", &is_open, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoInputs);
    
  ImGui::Text(aui_path);

  draw_ascii(aui);  
    
  ImGui::End();
}


void GUI :: draw_ascii(AUI *aui) {
  ascii_data_t* data = aui->getAsciiBuffer();
  for (auint i = 0; i < data->height; ++i) {
    for (auint j = 0; j < data->width; ++j) {
      char single_str[] = {data->data[i][j], '\0'};
      ImGui::Text(single_str);
      ImGui::SameLine(0.0f, 0.0f);
    }
    ImGui::Text("\n");
    free(data->data[i]);
  }
  free(data->data);
  free(data);
}


static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}