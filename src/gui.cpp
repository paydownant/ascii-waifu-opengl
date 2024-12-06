
#include <GLFW/glfw3.h>

#include "gui.hpp"
#include "gui_widgets.hpp"
#include "gui_helper.hpp"

#include "font_bin_compressed.hpp"
#include "icon_bin_pixel.hpp"

GUI :: GUI() {
  glsl_version = strdup("#version 330");

  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Get Platform
  platform = glfwGetPlatform();

  // Get Display Size
  int display_x = 10, display_y = 10, display_w = 1200, display_h = 800;
  glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &display_x, &display_y, &display_w, &display_h);

  window_w = display_w * .8f;
  window_h = display_h * .8f;

  window_style = DARK;
  window_title = strdup("AsciiWaifu");
  
  bg_colour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

  aui_path = strdup("input/path.image");

  custom_font_path = strdup("custom/font.ttf");

  draw_properties.ascii_set = strdup("#");
  draw_properties.ascii_scale = draw_properties.default_val.ascii_scale;
  draw_properties.aspect_ratio = draw_properties.default_val.aspect_ratio;
  draw_properties.ascii_font.size_slider = font_pixels.sizes[draw_properties.default_val.font_set_index];
  draw_properties.tool_window_size = 260;

  output_path = strdup("dest/path.image");

  // GLFW
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  window = glfwCreateWindow(window_w, window_h, window_title, nullptr, nullptr);
  if (window == nullptr) {
    glfw_error_callback(1, "Failed to Create GLFW Window");
  }
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Set Window Icon
  set_window_icon();

  // Setup ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

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

void GUI :: set_window_icon() {
  //Image image("../icon.png");
  //image.write_strip_hpp("../src/icon_bin_pixel.hpp");
  bool set_icon = false;
  switch (platform) {
    case GLFW_PLATFORM_WIN32:
      set_icon = true;
      break;
    case GLFW_PLATFORM_X11:
      set_icon = true;
      break;
    case GLFW_PLATFORM_UNAVAILABLE:
      set_icon = false;
      break;
    default:
      set_icon = false;
      break;
  }
  
  if (set_icon) {
    GLFWimage icon_image;
    icon_image.width = icon_image_pixel_size[0];
    icon_image.height = icon_image_pixel_size[1];
    icon_image.pixels = icon_image_pixel_data;

    glfwSetWindowIcon(window, 1, &icon_image);
  }
}

void GUI :: run() {
  // Get IO
  ImGuiIO &io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

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

    // Body End

    // Rendering
    ImGui::Render();

    // Process Input
    process_input();
    
    glfwGetFramebufferSize(window, &window_w, &window_h);
    glViewport(0, 0, window_w, window_h);
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
    bool load = aui->load_base_image(aui_path);
    if (load) {
      notifications.pool.push_back("Image Loaded!");
    } else {
      notifications.pool.push_back("Failed to Load Base Image");
    }
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
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2((float)window_w, (float)window_h));
  ImGui::SetNextWindowBgAlpha(0.0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
  ImGui::Begin("Render", NULL, flags);

  draw_ascii();
  draw_bounds();

  ImGui::PopStyleVar();
  ImGui::End();
}

void GUI :: tool_window() {
  ImGuiWindowFlags flags = 
  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar  |
  ImGuiWindowFlags_NoScrollbar        | ImGuiWindowFlags_NoDecoration          | ImGuiWindowFlags_NoResize    |
  ImGuiWindowFlags_NoMove             | ImGuiWindowFlags_NoSavedSettings       | ImGuiWindowFlags_NoNav;
  ImVec2 tool_pos = ImVec2(window_w - draw_properties.tool_window_size, 0);
  ImVec2 tool_size = ImVec2(draw_properties.tool_window_size, window_h);

  ImGui::SetNextWindowPos(tool_pos);
  ImGui::SetNextWindowSize(tool_size);
  ImGui::SetNextWindowBgAlpha(0.8);
  ImGui::PushFont(draw_properties.window_font.font);
  ImGui::Begin("Tools", NULL, flags);
  
  // Widgets
  widgets.pos = ImVec2(tool_pos.x + widgets.padding, tool_pos.y + widgets.padding);
  widgets.size = ImVec2(tool_size.x - 2 * widgets.padding, widgets.ratio * tool_size.y - 2 * widgets.padding);
  draw_widgets();
  // Widgets End

  // notifications
  notifications.pos = ImVec2(tool_pos.x + notifications.padding, tool_pos.y + widgets.pos.y + widgets.size.y + notifications.padding);
  notifications.size = ImVec2(tool_size.x - 2 * notifications.padding, (1 - widgets.ratio) * tool_size.y - 2 * notifications.padding);
  draw_notifications();
  // notifications End
  
  ImGui::End();
  ImGui::PopFont();
}

void GUI :: draw_widgets() {
  ImGuiWindowFlags flags =
  ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings;
  ImGui::SetNextWindowPos(widgets.pos);
  ImGui::SetNextWindowSize(widgets.size);
  ImGui::SetNextWindowBgAlpha(0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0); // no border
  ImGui::Begin("Widgets", NULL, flags);
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
  ImGui::PopStyleVar();
  ImGui::End();
}

void GUI :: draw_notifications() {
  ImGuiWindowFlags flags =
  ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings;
  ImGui::SetNextWindowPos(notifications.pos);
  ImGui::SetNextWindowSize(notifications.size);
  ImGui::SetNextWindowBgAlpha(0.8);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
  ImGui::Begin("notifications", NULL, flags);
  for (auto &note : notifications.pool) {
    ImGui::Text(note.c_str());
  }
  if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
    // Scroll to the Bottom
    ImGui::SetScrollHereY(1.0f);
  }
  ImGui::PopStyleVar();
  ImGui::End();
}


void GUI :: draw_ascii() {
  if (!aui->is_base_img_loaded()) {
    return;
  }
  ascii_data_t data;
  aui->getAsciiBuffer(&data, draw_properties.ascii_set, (unsigned int)strlen(draw_properties.ascii_set));

  ImGui::PushFont(draw_properties.ascii_font.font);
  ImVec2 tex_size = ImGui::CalcTextSize("O");
  float font_w = tex_size.x, font_h = tex_size.y;
  for (unsigned int y = 0; y < data.height; ++y) {
    for (unsigned int x = 0; x < data.width; ++x) {
      float r_level, g_level, b_level;
      r_level = data.colour_strip[y * data.width + x].x;
      g_level = data.colour_strip[y * data.width + x].y;
      b_level = data.colour_strip[y * data.width + x].z;
      
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r_level, g_level, b_level, 1.0f));
      ImGui::SetCursorPosX(font_w * x);
      ImGui::SetCursorPosY(font_h * y);
      ImGui::Text("%c", data.char_strip[y * data.width + x]);
      ImGui::PopStyleColor();

      ImGui::SameLine(0.0f, 0.0f);
    }
    ImGui::NewLine();
  }
  ImGui::PopFont();
  free(data.char_strip);
  free(data.colour_strip);
  
  // Calculating Drawing Boundary
  draw_properties.boundary.x_min = 0;
  draw_properties.boundary.y_min = 0;
  draw_properties.boundary.x_max = (strlen(draw_properties.ascii_set) > 0) * font_w * data.width + draw_properties.boundary.x_min;
  draw_properties.boundary.y_max = (strlen(draw_properties.ascii_set) > 0) * font_h * data.height + draw_properties.boundary.y_min;
  
  unsigned int drawable_max_x = window_w - draw_properties.tool_window_size;
  unsigned int drawable_max_y = window_h;
  // Check for Out of Bounds
  if (draw_properties.boundary.x_max > drawable_max_x || draw_properties.boundary.y_max > drawable_max_y) {
    widgets.shape_bounds = true;
  } else {
    widgets.shape_bounds = false;
  }
}

void GUI :: draw_bounds() {
  if (!widgets.shape_bounds) {
    return;
  }
  ImGuiWindowFlags flags = 
  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar      |
  ImGuiWindowFlags_NoResize           | ImGuiWindowFlags_NoMove                | ImGuiWindowFlags_NoSavedSettings | 
  ImGuiWindowFlags_NoScrollbar        | ImGuiWindowFlags_NoDecoration          | ImGuiWindowFlags_NoNav;

  unsigned int boundary_size_w = window_w - draw_properties.tool_window_size - draw_properties.boundary.x_min;
  unsigned int boundary_size_h = window_h - draw_properties.boundary.y_min;

  ImGui::SetNextWindowPos(ImVec2(draw_properties.boundary.x_min, draw_properties.boundary.y_min));
  ImGui::SetNextWindowSize(ImVec2(boundary_size_w, boundary_size_h));
  ImGui::SetNextWindowBgAlpha(0.0);

  ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 5.0f);
  
  ImGui::Begin("Bounds", NULL, flags);
  ImGui::End();

  ImGui::PopStyleVar();
  ImGui::PopStyleColor();
}

void GUI :: update_resolution() {
  draw_properties.resolution = 1.66f * draw_properties.ascii_scale
   * (window_w / draw_properties.ascii_font.size + ((window_w / draw_properties.ascii_font.size) * (.5f - draw_properties.aspect_ratio)));
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
    ImFont *new_ascii_font = font_atlas->AddFontFromMemoryCompressedTTF(pixelify_compressed_data, pixelify_compressed_size, size, font_config, font_atlas->GetGlyphRangesDefault());
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
    // custom fonts
    if (is_file_ttf(custom_font_path)) {
      font_pixels.fonts.clear();
      font_atlas->Clear();
      load_window_font();
      load_custom_ascii_fonts();
      notifications.pool.push_back("Fonts Loaded!");
    } else {
      fprintf(stderr, "File Format Specified is Not TTF\n");
      notifications.pool.push_back("Fonts Must be TTF");
      return;
    }
  }

  font_atlas->Build();
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();
}

void GUI :: export_img() {
  if (!aui->is_base_img_loaded()) {
    // Base Image Not Loaded
    fprintf(stderr, "Base Image is Not Loaded\n");
    notifications.pool.push_back("No Image to Export");
    return;
  }

  if (!draw_properties.boundary.x_max && !draw_properties.boundary.y_max) {
    // No Ascii Image to Export
    fprintf(stderr, "Ascii Image Not Rendered\n");
    notifications.pool.push_back("No Image to Export");
    return;
  }

  if (widgets.shape_bounds) {
    // Out of Bounds
    fprintf(stderr, "Ascii Image is Out of Bounds\n");
    notifications.pool.push_back("Resize the Image to Fit");
    return;
  }

  unsigned int channels = 4;
  unsigned int image_size_w = draw_properties.boundary.x_max - draw_properties.boundary.x_min;
  unsigned int image_size_h = draw_properties.boundary.y_max - draw_properties.boundary.y_min;

  unsigned int *buffer = new unsigned int[image_size_w * image_size_h * channels];
  if (buffer == nullptr) {
    fprintf(stderr, "Failed to Create Export Buffer\n");
    notifications.pool.push_back("Failed to Export");
    return;
  }

  glReadPixels(draw_properties.boundary.x_min, window_h - draw_properties.boundary.y_max, image_size_w, image_size_h, GL_RGBA, GL_UNSIGNED_INT, buffer);
  
  if (!export_buffer_to_img(image_size_w, image_size_h, channels, buffer, output_path)) {
    fprintf(stderr, "Failed to Export\n");
    notifications.pool.push_back("Failed to Export");
    return;
  }
  notifications.pool.push_back("Image Exported!");
}

void GUI :: clean_gui_mem() {
  // draw_properties
  free(draw_properties.ascii_set);

  // gui stuff
  free(custom_font_path);
  free(output_path);
  free(window_title);
  free(glsl_version);

  // font pixels stuff
  font_pixels.fonts.clear();

  // font atlas
  font_atlas->Clear();
}