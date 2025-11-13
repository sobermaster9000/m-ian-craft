#include "pandaFramework.h"
#include "pandaSystem.h"

int main(int argc, char *argv[]) {
  PandaFramework framework;
  framework.open_framework(argc, argv);

  framework.set_window_title("M-Ian-Craft");
  WindowFramework *window = framework.open_window();

  // more code here

  framework.main_loop();
  framework.close_framework();
  return (0);
}