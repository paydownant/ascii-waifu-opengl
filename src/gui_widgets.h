#ifndef GUI_WIDGETS_H
#define GUI_WIDGETS_H

#include "gui.h"

bool gui_path_load_button(GUI &gui, const char *label, char **path) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (11 + strlen(label)));
  snprintf(new_label, 11 + strlen(label), "##INPUT_TEXT%s", label);
  ImGui::InputText(new_label, *path, 256);
  ImGui::SameLine();
  snprintf(new_label, 6 + strlen(label), "Load %s", label);
  bool pressed = ImGui::Button(new_label);
  free(new_label);
  return pressed;
}

bool gui_text_input(GUI &gui, const char *label, char **text) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (11 + strlen(label)));
  snprintf(new_label, 11 + strlen(label), "##INPUT_TEXT%s", label);
  bool pressed = ImGui::InputText(new_label, *text, 256);
  return pressed;
}

bool gui_slider_int(GUI &gui, const char *label, int *value, int min, int max) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (9 + strlen(label)));
  snprintf(new_label, 9 + strlen(label), "##SLIDER%s", label);
  bool pressed = ImGui::SliderInt(new_label, &(*value), min, max, "%d px");
  free(new_label);
  return pressed;
}

bool gui_slider_float(GUI &gui, const char *label, float *value, float min, float max) {
  ImGui::Text(label);
  char *new_label = (char*)malloc(sizeof(char) * (9 + strlen(label)));
  snprintf(new_label, 9 + strlen(label), "##SLIDER%s", label);
  bool pressed = ImGui::SliderFloat(new_label, &(*value), min, max, "%.2f");
  free(new_label);
  return pressed;
}

#endif