#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdlib>
#include "viewer.hpp"
#include "game.hpp"

// print matrix in correct order
void printm(GLfloat* mvmatrix){
	  for(int j=0;j<4;j++){
	   printf("%.2f,%.2f,%.2f,%.2f",mvmatrix[j],mvmatrix[j+4],mvmatrix[j+8],mvmatrix[j+12]);
	   printf("\n");
	  }
}

// import function is taken from
//http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/
void Viewer::importbmp(){
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;
    const char* filename;
    for(int handle=0;handle<5;handle++){
        if(handle == 0) filename="pup.bmp";
        if(handle == 1) filename="pmd.bmp";
        if(handle == 2) filename="pdn.bmp";
        if(handle == 3) filename="bk";
        if(handle == 4) filename="lf";

        FILE * file = fopen(filename, "rb");

        if ( fread(header, 1, 54, file)!=54 ){ // If not 54 bytes read : problem
            printf("Not a correct BMP file\n");
            abort();
        }

        if ( header[0]!='B' || header[1]!='M' ){
            printf("Not a correct BMP file\n");
            abort();
        }

        // Read ints from the byte array
        dataPos    = *(int*)&(header[0x0A]);
        imageSize  = *(int*)&(header[0x22]);
        width      = *(int*)&(header[0x12]);
        height     = *(int*)&(header[0x16]);
        data = (unsigned char *)malloc( width * height * 4 );

        if (imageSize==0)    imageSize=width*height*4; // 3 : one byte for each Red, Green and Blue component
        if (dataPos==0)      dataPos=54;

        data = new unsigned char [imageSize];

        // Read the actual data from the file into the buffer
        fread(data,1,imageSize,file);

        //Everything is in memory now, the file can be closed
        fclose(file);

        glGenTextures(1, &sb[handle]);

        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, sb[handle]);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Give the image to OpenGL
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
    }
}

Viewer::Viewer()
{
    Glib::RefPtr<Gdk::GL::Config> glconfig;

    // Ask for an OpenGL Setup with
    //  - red, green and blue component colour
    //  - a depth buffer to avoid things overlapping wrongly
    //  - double-buffered rendering to avoid tearing/flickering
    glconfig = Gdk::GL::Config::create(Gdk::GL::MODE_RGB |
                                     Gdk::GL::MODE_DEPTH |
                                     Gdk::GL::MODE_DOUBLE);
    if (glconfig == 0) {
    // If we can't get this configuration, die
    std::cerr << "Unable to setup OpenGL Configuration!" << std::endl;
    abort();
    }

    // Accept the configuration
    set_gl_capability(glconfig);

    // Register the fact that we want to receive these events
    add_events(Gdk::BUTTON1_MOTION_MASK    |
             Gdk::BUTTON2_MOTION_MASK    |
             Gdk::BUTTON3_MOTION_MASK    |
             Gdk::BUTTON_PRESS_MASK      | 
             Gdk::BUTTON_RELEASE_MASK    |
             Gdk::VISIBILITY_NOTIFY_MASK);

    for(int i=0;i<5;i++){
        sb[i] = 0;
    }

    makebuilding();
    sbox = 0;
}

void Viewer::makebuilding(){
    building = new GeometryNode("building",new Cube());
    building->set_transform(Matrix4x4());
    building->scale(Vector3D(1.25,0.48,1));
    building->m_material = new PhongMaterial(Colour(1,0.18,0.18),Colour(0.4,0.4,0.4),0.5);
    building->translate(Vector3D(-0.5,-1.18,0));

    GeometryNode* roof = new GeometryNode("roof", new Triangle());
    building->add_child(roof);
    roof->scale(Vector3D(1.2,1,1));
    roof->translate(Vector3D(-0.08,1,0));
}

Viewer::~Viewer()
{
    for(int i=0;i<5;i++)
        glDeleteTextures(1, &sb[i]);

    delete building->m_children.front();
    delete building;
}

void Viewer::setGame(Game* g){
	m_game = g;
	m_game->view = this;
}

void Viewer::invalidate()
{
    Gtk::Allocation allocation = get_allocation();
    get_window()->invalidate_rect(allocation, false);
}

void Viewer::on_realize()
{
  // Do some OpenGL setup.
  // First, let the base class do whatever it needs to
  Gtk::GL::DrawingArea::on_realize();
  
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
  
  if (!gldrawable)
    return;

  if (!gldrawable->gl_begin(get_gl_context()))
    return;

  // Just enable depth testing and set the background colour.
  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  gldrawable->gl_end();
}

void Viewer::drawbackground(){
    building->walk_gl();
}

void Viewer::skybox(){
    glPushMatrix();
    // Enable/Disable features
    glPushAttrib(GL_ENABLE_BIT);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_BLEND);

    if(sb[0] == 0){
        importbmp();
    }
    glEnable(GL_TEXTURE_2D);

    glColor4f(1,1,1,1);
    glBindTexture(GL_TEXTURE_2D, sb[4]);

    // back
    glBegin(GL_QUADS);
    glNormal3f(0,0,1);
    glTexCoord2f(1,1); glVertex3f(-1,1,-1);
    glTexCoord2f(0,1); glVertex3f(1,1,-1);
    glTexCoord2f(0,0); glVertex3f(1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, sb[1]);
    glBegin(GL_QUADS);
    glNormal3f(-1,0,0);
    glTexCoord2f(1,0); glVertex3f(1,-1,1);
    glTexCoord2f(1,1); glVertex3f(1,1,1);
    glTexCoord2f(0,1); glVertex3f(1,1,-1);
    glTexCoord2f(0,0); glVertex3f(1,-1,-1);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, sb[3]);
    glBegin(GL_QUADS);
    glNormal3f(1,0,0);
    glTexCoord2f(1,0); glVertex3f(-1,-1,1);
    glTexCoord2f(1,1); glVertex3f(-1,1,1);
    glTexCoord2f(0,1); glVertex3f(-1,1,-1);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glEnd();

    // sky
    glBindTexture(GL_TEXTURE_2D, sb[0]);
    glBegin(GL_QUADS);
    glNormal3f(0, -1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,1,-1);
    glTexCoord2f(1,0); glVertex3f(1,1,-1);
    glTexCoord2f(1,1); glVertex3f(1,1,1);
    glTexCoord2f(0,1); glVertex3f(-1,1,1);
    glEnd();

    // ground
    glBindTexture(GL_TEXTURE_2D, sb[2]);
    glBegin(GL_QUADS);
    glNormal3f(0, 1, 0);
    glTexCoord2f(1,1); glVertex3f(-1,-1,-1);
    glTexCoord2f(0,1); glVertex3f(1,-1,-1);
    glTexCoord2f(0,0); glVertex3f(1,-1,1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,1);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glClear(GL_DEPTH_BUFFER_BIT);

    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

bool Viewer::on_expose_event(GdkEventExpose* event)
{
    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

    if (!gldrawable) return false;

    if (!gldrawable->gl_begin(get_gl_context()))
      return false;
    draw();

    return true;
}

void Viewer::draw(){
    Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(90.0, 1, 0.5, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLoadIdentity();
    gluLookAt(0,0,0.7,0,0,0,0,1,0);

    skybox();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, -1.0f);

    glDepthFunc(GL_ALWAYS);
    glColor3f(1,0.18,0.18);
    glBegin(GL_QUADS);
    glVertex3d(-0.625, -0.57, 1);
    glVertex3d(-0.625, 0, 1);
    glVertex3d(0.625, 0, 1);
    glVertex3d(0.625, -0.57, 1);

    glEnd();
    glDepthFunc(GL_LEQUAL);
    // Set up lighting
    glEnable(GL_LIGHTING);
    GLfloat light[] = {1,1,1,1};
    GLfloat light_position[] = {50,100,200,1};
    glLightfv(GL_LIGHT0,GL_DIFFUSE,light);
    glLightfv(GL_LIGHT0,GL_POSITION,light_position);
    glEnable(GL_LIGHT0);

    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // Draw stuff
    if(!m_game->stop)
        m_game->render();
    else
        m_game->renderfinal();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    drawbackground();
    glDisable(GL_BLEND);

    glPopMatrix();

    if(sbox){
        skybox();
    }
    // Swap the contents of the front and back buffers so we see what we
    // just drew. This should only be done if double buffering is enabled.
    gldrawable->swap_buffers();

    gldrawable->gl_end();
}

bool Viewer::on_configure_event(GdkEventConfigure* event)
{
  Glib::RefPtr<Gdk::GL::Drawable> gldrawable = get_gl_drawable();

  if (!gldrawable) return false;
  
  if (!gldrawable->gl_begin(get_gl_context()))
    return false;

  // Set up perspective projection, using current size and aspect1
  // ratio of display

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glViewport(0, 0, event->width, event->height);
  gluPerspective(40.0, (GLfloat)event->width/(GLfloat)event->height, 0.1, 1000.0);

  // Reset to modelview matrix mode
  glMatrixMode(GL_MODELVIEW);

  gldrawable->gl_end();

  return true;
}

bool Viewer::on_button_press_event(GdkEventButton* event)
{
  return true;
}

bool Viewer::on_button_release_event(GdkEventButton* event)
{
  return true;
}

bool Viewer::on_motion_notify_event(GdkEventMotion* event)
{
  return true;
}
