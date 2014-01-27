#ifndef APPWINDOW_HPP
#define APPWINDOW_HPP

#include <gtkmm.h>
#include "viewer.hpp"
#include "game.hpp"

class AppWindow : public Gtk::Window {
public:
  AppWindow();
  virtual ~AppWindow(){	delete m_game; delete m_viewer; }
  Viewer* getView();
  void run(int interval = 50);
  void reset();
  void newGame();
  int getSpeed();
  void sbox();

  virtual bool on_key_press_event( GdkEventKey *ev );

  Gtk::VBox m_vbox;

  Gtk::MenuBar m_menubar;
  Gtk::Menu m_menu_app;
  Gtk::Label lb;

  Viewer* m_viewer;
  Game* m_game;
  sigc::connection m_con;
  sigc::connection m_walk;
  int m_speed;
};

#endif
