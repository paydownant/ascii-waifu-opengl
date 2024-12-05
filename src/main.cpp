
#include "gui.hpp"

int main(int argc, char **argv) {
  GUI *gui = new GUI();
  gui->run();
  delete(gui);
}