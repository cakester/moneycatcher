#include "appwindow.hpp"
#include <iostream>

AppWindow::AppWindow()
{
    set_title("Money Catcher");

    // A utility class for constructing things that go into menus, which
    // we'll set up next.
    using Gtk::Menu_Helpers::MenuElem;

    // Set up the application menu
    // The slot we use here just causes AppWindow::hide() on this,
    // which shuts down the application.
    m_viewer = new Viewer();

    m_menu_app.items().push_back(MenuElem("_Quit", Gtk::AccelKey("q"),
    sigc::mem_fun(*this, &AppWindow::hide)));
    m_menu_app.items().push_back(MenuElem("_New game", Gtk::AccelKey("n"),
          sigc::mem_fun(*this, &AppWindow::newGame)));

    m_menu_app.items().push_back(MenuElem("_skybox", Gtk::AccelKey("s"),
              sigc::mem_fun(*this, &AppWindow::sbox)));

    m_menubar.items().push_back(Gtk::Menu_Helpers::MenuElem("_Application", m_menu_app));

    // Pack in our widgets

    // First add the vertical box as our single "top" widget
    add(m_vbox);

    // Put the menubar on the top, and make it as small as possible
    m_vbox.pack_start(m_menubar, Gtk::PACK_SHRINK);

    // Put the viewer below the menubar. pack_start "grows" the widget
    // by default, so it'll take up the rest of the window.
    m_viewer->set_size_request(800, 600);
    m_vbox.pack_start(lb, Gtk::PACK_SHRINK);
    m_viewer->lb = &lb;
    m_vbox.pack_start(*m_viewer);

    m_game = new Game();
    m_viewer->setGame(m_game);

    show_all();
}

Viewer* AppWindow::getView(){
	return m_viewer;
}

int AppWindow::getSpeed(){
	return m_speed;
}

void AppWindow::run(int interval){
	sigc::slot0<int> aslot=sigc::mem_fun(*m_game, &Game::tick);
	if(!m_con.connected()){
		  m_con =Glib::signal_timeout().connect(aslot, interval);
	}
	else{
		m_con.disconnect();
		m_con =Glib::signal_timeout().connect(aslot, interval);
	}
	m_speed = interval;
}

void AppWindow::reset(){
	m_viewer->invalidate();
}

void AppWindow::newGame(){
	if(m_game)	m_game->reset();
	run();
}

void AppWindow::sbox(){
    m_viewer->sbox = !m_viewer->sbox;
}

bool AppWindow::on_key_press_event( GdkEventKey *ev )
{
    bool b;
	switch(ev->keyval){
	case GDK_Left:
	    if(!m_game->stop){
            m_game->pc.move(-1);
            if(m_game->drop(0)){
                m_game->stop = 1;
            }
	    }
		break;
	case GDK_Right:
        if(!m_game->stop){
            m_game->pc.move(1);
            if(m_game->drop(0)){
                m_game->stop = 1;
            }
        }
		break;
	case GDK_Up:
        if(!m_game->stop){
            m_game->pc.hand = 1;
            m_game->handon = !m_game->handon;
            if(m_game->drop(0)){
                m_game->stop = 1;
            }
        }
		break;
	case GDK_Down:
		break;
	case GDK_space:
		break;
	default:
		b = Gtk::Window::on_key_press_event( ev );
		break;
	}

    m_viewer->invalidate();
	return b;
}
