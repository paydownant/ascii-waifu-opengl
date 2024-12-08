
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
  ui.platform = glfwGetPlatform();

  // Get Display Size
  int display_x = 10, display_y = 10, display_w = 1200, display_h = 800;
  glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), &display_x, &display_y, &display_w, &display_h);

  ui.window_w = display_w * .8f;
  ui.window_h = display_h * .8f;

  ui.window_style = DARK;
  ui.window_title = strdup("AsciiWaifu");
  
  ui.bg_colour = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

  aui_path = strdup("input/path.image");

  custom_font_path = strdup("custom/font.ttf");

  ui.ascii_set = strdup("#");
  ui.ascii_scale = ui.default_val.ascii_scale;
  ui.aspect_ratio = ui.default_val.aspect_ratio;
  ui.ascii_font.size_slider = ui.font_pixels.sizes[ui.default_val.font_set_index];
  ui.tool_window_size = 260;

  output_path = strdup("dest/path.image");

  // GLFW
  GLFWmonitor *primary_monitor = glfwGetPrimaryMonitor();
  ui.window = glfwCreateWindow(ui.window_w, ui.window_h, ui.window_title, nullptr, nullptr);
  if (ui.window == nullptr) {
    glfw_error_callback(1, "Failed to Create GLFW Window");
  }
  glfwMakeContextCurrent(ui.window);
  glfwSwapInterval(1);

  // Set Window Icon
  set_window_icon();

  // Setup ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  // Setup ImGui style
  switch (ui.window_style) {
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

  // Setup ui.Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(ui.window, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

GUI :: ~GUI() {
  printf("Terminating GUI\n");
  // Clean up
  delete(aui);

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(ui.window);
  glfwTerminate();

  clean_gui_mem();
}

void GUI :: set_window_icon() {
  //Image image("../icon.png");
  //image.write_strip_hpp("../src/icon_bin_pixel.hpp");
  bool set_icon = false;
  switch (ui.platform) {
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

    glfwSetWindowIcon(ui.window, 1, &icon_image);
  }
}

void GUI :: run() {
  // Get IO
  ImGuiIO &io = ImGui::GetIO(); (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  // Load fonts here
  ui.font_atlas = io.Fonts;
  ImFontConfig fc;
  ui.font_config = &fc;

  load_fonts();

  // State
  ImVec4 clear_color = ui.bg_colour;

  // Initiate AUI
  aui = new AUI();

  //aui->load_base_image(aui_path);
  //aui->createVertexBuffer(ui.resolution, ui.aspect_ratio);

  update_resolution();

  // Main Loop
  while (!glfwWindowShouldClose(ui.window)) {
    
    // Poll and handle events
    glfwPollEvents();
    if (glfwGetWindowAttrib(ui.window, GLFW_ICONIFIED) != 0) {
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
    
    glfwGetFramebufferSize(ui.window, &ui.window_w, &ui.window_h);
    glViewport(0, 0, ui.window_w, ui.window_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(ui.window);
  }
}

void GUI :: process_input() {
  if (glfwGetKey(ui.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(ui.window, GLFW_TRUE);
  }

  bool pend_update_buffer = false;

  if (ui.widgets.button_load_base_image) {
    pend_update_buffer = true;
    bool load = aui->load_base_image(aui_path);
    if (load) {
      ui.notifications.pool.push_back("Image Loaded!");
    } else {
      ui.notifications.pool.push_back("Failed to Load Base Image");
    }
  }

  if (ui.widgets.slider_scale || ui.widgets.slider_aspect_ratio) {
    // update vertex buffer
    pend_update_buffer = true;
  }

  if (ui.widgets.button_reset_scale) {
    pend_update_buffer = true;
    ui.ascii_scale = ui.default_val.ascii_scale;
  }

  if (ui.widgets.button_reset_aspect_ratio) {
    pend_update_buffer = true;
    ui.aspect_ratio = ui.default_val.aspect_ratio;
  }

  if (ui.widgets.slider_font_size) {
    // update font size
    pend_update_buffer = true;
    update_font_size();
  }

  if (ui.widgets.button_reset_font_size) {
    pend_update_buffer = true;
    ui.ascii_font.size_slider = ui.font_pixels.sizes[ui.default_val.font_set_index];
    update_font_size();
  }

  if (ui.widgets.input_ascii_char) {
    // no need to implement for now since ascii_set is updated every frame
    pend_update_buffer = true;
  }

  if (ui.widgets.button_load_custom_font) {
    // update font
    pend_update_buffer = true;
    load_fonts();
    update_font_size();
  }
  
  // GUI update should be placed above
  if (pend_update_buffer) {
    if (aui->is_base_img_loaded()) {
      update_resolution();
      aui->createVertexBuffer(ui.resolution, ui.aspect_ratio);
    }
  }

  if (ui.widgets.button_export_img) {
    export_img();
  }
}

void GUI :: ascii_window() {
  ImGuiWindowFlags flags = 
  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar  |
  ImGuiWindowFlags_NoScrollbar        | ImGuiWindowFlags_NoDecoration          | ImGuiWindowFlags_NoResize    |
  ImGuiWindowFlags_NoMove             | ImGuiWindowFlags_NoSavedSettings       | ImGuiWindowFlags_NoNav;
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImGui::SetNextWindowSize(ImVec2((float)ui.window_w, (float)ui.window_h));
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
  ImVec2 tool_pos = ImVec2(ui.window_w - ui.tool_window_size, 0);
  ImVec2 tool_size = ImVec2(ui.tool_window_size, ui.window_h);

  ImGui::SetNextWindowPos(tool_pos);
  ImGui::SetNextWindowSize(tool_size);
  ImGui::SetNextWindowBgAlpha(0.8);
  ImGui::PushFont(ui.window_font.font);
  ImGui::Begin("Tools", NULL, flags);
  
  // ui.Widgets
  ui.widgets.pos = ImVec2(tool_pos.x + ui.widgets.padding, tool_pos.y + ui.widgets.padding);
  ui.widgets.size = ImVec2(tool_size.x - 2 * ui.widgets.padding, ui.widgets.ratio * tool_size.y - 2 * ui.widgets.padding);
  draw_widgets();
  // ui.Widgets End

  // ui.notifications
  ui.notifications.pos = ImVec2(tool_pos.x + ui.notifications.padding, tool_pos.y + ui.widgets.pos.y + ui.widgets.size.y + ui.notifications.padding);
  ui.notifications.size = ImVec2(tool_size.x - 2 * ui.notifications.padding, (1 - ui.widgets.ratio) * tool_size.y - 2 * ui.notifications.padding);
  draw_notifications();
  // ui.notifications End
  
  ImGui::End();
  ImGui::PopFont();
}

void GUI :: draw_widgets() {
  ImGuiWindowFlags flags =
  ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings;
  ImGui::SetNextWindowPos(ui.widgets.pos);
  ImGui::SetNextWindowSize(ui.widgets.size);
  ImGui::SetNextWindowBgAlpha(0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0); // no border
  ImGui::Begin("ui.Widgets", NULL, flags);
  ui.widgets.button_load_base_image = gui_path_load_button(*this, "Image Path", &aui_path);
  
  ui.widgets.slider_scale = gui_slider_float(*this, "Scale", &ui.ascii_scale, 0.10, 3.00);
  ui.widgets.button_reset_scale = gui_reset_button_sameline(*this, "Scale");

  ui.widgets.slider_aspect_ratio = gui_slider_float(*this, "Aspect Ratio", &ui.aspect_ratio, 0.01, 0.99);
  ui.widgets.button_reset_aspect_ratio = gui_reset_button_sameline(*this, "Aspect Ratio");

  ui.widgets.slider_font_size = gui_slider_int(*this, "Font Size", &ui.ascii_font.size_slider, ui.font_pixels.sizes.front(), ui.font_pixels.sizes.back());
  ui.widgets.button_reset_font_size = gui_reset_button_sameline(*this, "Font Size");

  ui.widgets.input_ascii_char = gui_text_input(*this, "Ascii Set", &ui.ascii_set);
  
  ui.widgets.button_load_custom_font = gui_path_load_button(*this, "Custom Font Path", &custom_font_path);

  ui.widgets.button_export_img = gui_export_img_button(*this, "Export Window as Image", &output_path);
  ImGui::PopStyleVar();
  ImGui::End();
}

void GUI :: draw_notifications() {
  ImGuiWindowFlags flags =
  ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoSavedSettings;
  ImGui::SetNextWindowPos(ui.notifications.pos);
  ImGui::SetNextWindowSize(ui.notifications.size);
  ImGui::SetNextWindowBgAlpha(0.0);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);
  ImGui::Begin("ui.notifications", NULL, flags);
  for (auto &note : ui.notifications.pool) {
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
  aui->getAsciiBuffer(&data, ui.ascii_set, (unsigned int)strlen(ui.ascii_set));

  ImGui::PushFont(ui.ascii_font.font);
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
  ui.boundary.x_min = 0;
  ui.boundary.y_min = 0;
  ui.boundary.x_max = (strlen(ui.ascii_set) > 0) * font_w * data.width + ui.boundary.x_min;
  ui.boundary.y_max = (strlen(ui.ascii_set) > 0) * font_h * data.height + ui.boundary.y_min;
  
  unsigned int drawable_max_x = ui.window_w - ui.tool_window_size;
  unsigned int drawable_max_y = ui.window_h;
  // Check for Out of Bounds
  if (ui.boundary.x_max > drawable_max_x || ui.boundary.y_max > drawable_max_y) {
    ui.widgets.shape_bounds = true;
  } else {
    ui.widgets.shape_bounds = false;
  }
}

void GUI :: draw_bounds() {
  if (!ui.widgets.shape_bounds) {
    return;
  }
  ImGuiWindowFlags flags = 
  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar      |
  ImGuiWindowFlags_NoResize           | ImGuiWindowFlags_NoMove                | ImGuiWindowFlags_NoSavedSettings | 
  ImGuiWindowFlags_NoScrollbar        | ImGuiWindowFlags_NoDecoration          | ImGuiWindowFlags_NoNav;

  unsigned int boundary_size_w = ui.window_w - ui.tool_window_size - ui.boundary.x_min;
  unsigned int boundary_size_h = ui.window_h - ui.boundary.y_min;

  ImGui::SetNextWindowPos(ImVec2(ui.boundary.x_min, ui.boundary.y_min));
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
  ui.resolution = 1.66f * ui.ascii_scale
   * (ui.window_w / ui.ascii_font.size + ((ui.window_w / ui.ascii_font.size) * (.5f - ui.aspect_ratio)));
}

void GUI :: update_font_size() {
  int fp = 0;
  for (auto size : ui.font_pixels.sizes) {
    if (size == ui.ascii_font.size_slider) {
      ui.ascii_font.size = size;
      ui.ascii_font.font = ui.font_pixels.fonts[fp];
      break;
    }
    fp++;
  }
  ui.ascii_font.size_slider = ui.ascii_font.size;
}

void GUI :: load_window_font() {
  ui.window_font.font = ui.font_atlas->AddFontFromMemoryCompressedTTF(opensans_compressed_data, opensans_compressed_size, ui.window_font.size, ui.font_config, ui.font_atlas->GetGlyphRangesDefault());
}

void GUI :: load_ascii_fonts() {
  unsigned int i = 0;
  for (auto size : ui.font_pixels.sizes) {
    ImFont *new_ascii_font = ui.font_atlas->AddFontFromMemoryCompressedTTF(pixelify_compressed_data, pixelify_compressed_size, size, ui.font_config, ui.font_atlas->GetGlyphRangesDefault());
    new_ascii_font->FontSize = size;
    ui.font_pixels.fonts.push_back(new_ascii_font);
    i++;
  }

  if (ui.font_pixels.fonts.size() != ui.font_pixels.sizes.size()) {
    fprintf(stderr, "Font Load Error: Unmatched Font Number\n");
    return;
  }
}

void GUI :: load_custom_ascii_fonts() {
  unsigned int i = 0;
  for (auto size : ui.font_pixels.sizes) {
    ImFont *new_ascii_font = ui.font_atlas->AddFontFromFileTTF(custom_font_path, size, ui.font_config, ui.font_atlas->GetGlyphRangesDefault());
    new_ascii_font->FontSize = size;
    ui.font_pixels.fonts.push_back(new_ascii_font);
    i++;
  }

  if (ui.font_pixels.fonts.size() != ui.font_pixels.sizes.size()) {
    fprintf(stderr, "Font Load Error: Unmatched Font Number\n");
    return;
  }
  ui.font_loaded = true;
}

void GUI :: load_fonts() {
  if (!ui.font_loaded) {
    load_window_font();
    load_ascii_fonts();
    ui.ascii_font.font = ui.font_pixels.fonts[ui.default_val.font_set_index];
    ui.ascii_font.size = ui.font_pixels.sizes[ui.default_val.font_set_index];
    ui.ascii_font.size_slider = ui.font_pixels.sizes[ui.default_val.font_set_index];
    ui.font_loaded = true;
  } else {
    // custom fonts
    if (is_file_ttf(custom_font_path)) {
      ui.font_pixels.fonts.clear();
      ui.font_atlas->Clear();
      load_window_font();
      load_custom_ascii_fonts();
      ui.notifications.pool.push_back("Fonts Loaded!");
    } else {
      fprintf(stderr, "File Format Specified is Not TTF\n");
      ui.notifications.pool.push_back("Fonts Must be TTF");
      return;
    }
  }

  ui.font_atlas->Build();
  ImGui_ImplOpenGL3_DestroyFontsTexture();
  ImGui_ImplOpenGL3_CreateFontsTexture();
}

void GUI :: export_img() {
  if (!aui->is_base_img_loaded()) {
    // Base Image Not Loaded
    fprintf(stderr, "Base Image is Not Loaded\n");
    ui.notifications.pool.push_back("No Image to Export");
    return;
  }

  if (!ui.boundary.x_max && !ui.boundary.y_max) {
    // No Ascii Image to Export
    fprintf(stderr, "Ascii Image Not Rendered\n");
    ui.notifications.pool.push_back("No Image to Export");
    return;
  }

  if (ui.widgets.shape_bounds) {
    // Out of Bounds
    fprintf(stderr, "Ascii Image is Out of Bounds\n");
    ui.notifications.pool.push_back("Resize the Image to Fit");
    return;
  }

  unsigned int channels = 4;
  unsigned int image_size_w = ui.boundary.x_max - ui.boundary.x_min;
  unsigned int image_size_h = ui.boundary.y_max - ui.boundary.y_min;

  unsigned int *buffer = new unsigned int[image_size_w * image_size_h * channels];
  if (buffer == nullptr) {
    fprintf(stderr, "Failed to Create Export Buffer\n");
    ui.notifications.pool.push_back("Failed to Export");
    return;
  }

  glReadPixels(ui.boundary.x_min, ui.window_h - ui.boundary.y_max, image_size_w, image_size_h, GL_RGBA, GL_UNSIGNED_INT, buffer);
  
  if (!export_buffer_to_img(image_size_w, image_size_h, channels, buffer, output_path)) {
    fprintf(stderr, "Failed to Export\n");
    ui.notifications.pool.push_back("Failed to Export");
    return;
  }
  ui.notifications.pool.push_back("Image Exported!");
}

void GUI :: clean_gui_mem() {
  // ui
  free(ui.ascii_set);

  // gui stuff
  free(custom_font_path);
  free(output_path);
  free(ui.window_title);
  free(glsl_version);

  // font pixels stuff
  ui.font_pixels.fonts.clear();

  // font atlas
  ui.font_atlas->Clear();
}