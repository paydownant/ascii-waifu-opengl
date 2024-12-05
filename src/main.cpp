
#include "gui.hpp"

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main(int argc, char **argv) {
  GUI *gui = new GUI();
  gui->run();
  delete(gui);
}