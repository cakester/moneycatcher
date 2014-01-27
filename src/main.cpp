#include <gtkmm.h>
#include <gtkglmm.h>
#include "appwindow.hpp"

int main(int argc, char** argv)
{
  // Construct our main loop
  Gtk::Main kit(argc, argv);

  // Initialize OpenGL
  Gtk::GL::init(argc, argv);

  std::string pcf = "pc.lua";
  SceneNode* pc = import_lua(pcf);

  // Construct our (only) window
  AppWindow window;
  window.m_game->pc.scene = pc;

  window.run();

  // And run the application!
  Gtk::Main::run(window);
}

