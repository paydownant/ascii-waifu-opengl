#ifndef GUI_WIDGETS_HPP
#define GUI_WIDGETS_HPP

#include "gui.hpp"

bool gui_path_load_button(GUI &gui, const char *label, char **path) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (11 + strlen(label)));
  snprintf(new_label, 11 + strlen(label), "##INPUT_TEXT%s", label);
  ImGui::InputText(new_label, *path, 256);
  ImGui::SameLine();
  snprintf(new_label, 7 + strlen(label), "Load##%s", label);
  bool pressed = ImGui::Button(new_label);
  free(new_label);
  return pressed;
}

bool gui_text_input(GUI &gui, const char *label, char **text) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (12 + strlen(label)));
  snprintf(new_label, 12 + strlen(label), "##INPUT_TEXT_%s", label);
  bool pressed = ImGui::InputText(new_label, *text, 256);
  return pressed;
}

bool gui_slider_int(GUI &gui, const char *label, int *value, int min, int max) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (10 + strlen(label)));
  snprintf(new_label, 10 + strlen(label), "##SLIDER_%s", label);
  bool pressed = ImGui::SliderInt(new_label, &(*value), min, max, "%d px");
  free(new_label);
  return pressed;
}

bool gui_slider_float(GUI &gui, const char *label, float *value, float min, float max) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (10 + strlen(label)));
  snprintf(new_label, 10 + strlen(label), "##SLIDER_%s", label);
  bool pressed = ImGui::SliderFloat(new_label, &(*value), min, max, "%.2f");
  free(new_label);
  return pressed;
}

bool gui_reset_button(GUI &gui, const char *label) {
  char *new_label = (char*)malloc(sizeof(char) * (8 + strlen(label)));
  snprintf(new_label, 8 + strlen(label), "Reset##%s", label);
  bool pressed = ImGui::Button(new_label);
  free(new_label);
  return pressed;
}

bool gui_reset_button_sameline(GUI &gui, const char *label) {
  char *new_label = (char*)malloc(sizeof(char) * (8 + strlen(label)));
  snprintf(new_label, 8 + strlen(label), "Reset##%s", label);
  ImGui::SameLine();
  bool pressed = ImGui::Button(new_label);
  free(new_label);
  return pressed;
}

bool gui_export_img_button(GUI &gui, const char *label, char **path) {
  ImGui::Text(label);
  ImGui::InputText("##INPUT_IMG", *path, 256);
  ImGui::SameLine();
  bool pressed = ImGui::Button("Export");
  return pressed;
}

#endif