#ifndef CS488_VIEWER_HPP
#define CS488_VIEWER_HPP

#include <gtkmm.h>
#include <gtkglmm.h>
#include <vector>
#include "scene_lua.hpp"

// The "main" OpenGL widget
class Game;
class Viewer : public Gtk::GL::DrawingArea {
public:
    Viewer();
    virtual ~Viewer();

    // A useful function that forces this widget to rerender. If you
    // want to render a new frame, do not call on_expose_event
    // directly. Instead call this, which will cause an on_expose_event
    // call when the time is right.
    void invalidate();
    void setup();
    void skybox();
    void drawbackground();
    void setGame(Game* g);
    void importbmp();
    void makebuilding();
    void renderpc();
    void draw();

    // Called when GL is first initialized
    virtual void on_realize();
    // Called when our window needs to be redrawn
    virtual bool on_expose_event(GdkEventExpose* event);
    // Called when the window is resized
    virtual bool on_configure_event(GdkEventConfigure* event);
    // Called when a mouse button is pressed
    virtual bool on_button_press_event(GdkEventButton* event);
    // Called when a mouse button is released
    virtual bool on_button_release_event(GdkEventButton* event);
    // Called when the mouse moves
    virtual bool on_motion_notify_event(GdkEventMotion* event);

    Game* m_game;
    Gtk::Label* lb;
    GLuint sb[5];
    GeometryNode* building;
    bool sbox;
};

#endif
