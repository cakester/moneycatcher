#include "primitive.hpp"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <fstream>

int initt = 0;
int part = 1000;

Primitive::~Primitive()
{
}

Sphere::~Sphere()
{
}

void Sphere::walk_gl()
{
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);
    gluQuadricDrawStyle(q, GLU_FILL);
    gluSphere(q,1,20,20);
}

Bomb::Bomb(){
    newptc();
}

void Bomb::newptc(){
    for(int i=0;i<part;i++){
        particle pc;
        p.push_back(pc);
    }
}

void Bomb::walk_gl(){
    bool l = 0;
    if(glIsEnabled(GL_LIGHTING) == GL_TRUE){
        glDisable(GL_LIGHTING);
        l = 1;
    }
    glDepthFunc(GL_ALWAYS);
    glColor3f(1,0.65,0);
    for(int i=0;i<p.size();i++){
        glPushMatrix();
        p[i].refresh();
        p[i].render();
        glPopMatrix();
    }
    glDepthFunc(GL_LEQUAL);
    if(l){
        glEnable(GL_LIGHTING);
    }

    glColor3f(0,0,0);

    GLfloat c[4];
    c[0] = 0.01;
    c[1] = 0.01;
    c[2] = 0.01;
    c[3] = 1;

    glMaterialfv(GL_FRONT, GL_DIFFUSE, c);

    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);
    gluQuadricDrawStyle(q, GLU_FILL);
    gluSphere(q,1,20,20);

    glLineWidth(4);
    glBegin(GL_LINES);
    glVertex3d(0.0,1.0,0.0);
    glVertex3d(0.0,1.5,0.0);
    glEnd();
}

void Triangle::loadtext(){
    unsigned char header[54]; // Each BMP file begins by a 54-bytes header
    unsigned int dataPos;     // Position in the file where the actual data begins
    unsigned int width, height;
    unsigned int imageSize;   // = width*height*3
    // Actual RGB data
    unsigned char * data;
    const char* filename;
    filename = "a.bmp";

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
    data = (unsigned char *)malloc( width * height * 3 );

    if (imageSize==0)    imageSize=width*height*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54;

    data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(data,1,imageSize,file);

    //Everything is in memory now, the file can be closed
    fclose(file);

    glGenTextures(1, &texture);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Give the image to OpenGL
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
}

void Triangle::walk_gl(){
    if(initt == 0){
        loadtext();
        initt = 1;
    }

    GLfloat cc[4];
    cc[0] = 1;
    cc[1] = 1;
    cc[2] = 1;
    cc[3] = 1;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, cc);

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D, texture);

    glBegin(GL_QUADS);
    glNormal3d(0, 0, 1);
    glTexCoord2f(0,2.5f);glVertex3d(0.0, 1.0, 0.8);
    glTexCoord2f(2.5f,2.5f); glVertex3d(1.0, 1.0, 0.8);
    glTexCoord2f(2.5f,0); glVertex3d(1.0, 0.0, 1.0);
    glTexCoord2f(0,0); glVertex3d(0.0, 0.0, 1.0);

    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void Cube::walk_gl(){
    GLfloat c[4];
    c[0] = 1;
    c[1] = 0.18;
    c[2] = 0.18;
    c[3] = 0.8;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, c);
    double xx = 0.0, yy=0.0;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBegin(GL_QUADS);

    glNormal3d(0, 0, 1);
    glVertex3d(xx, yy, 1.0);
    glVertex3d(xx+1.0, yy, 1.0);
    glVertex3d(xx+1.0, yy+1.0, 1.0);
    glVertex3d(xx, yy+1.0, 1.0);
    glEnd();

    glNormal3d(0, 0, -1);
    glVertex3d(xx, yy, 0.0);
    glVertex3d(xx+1.0, yy, 0.0);
    glVertex3d(xx+1.0, yy+1.0, 0.0);
    glVertex3d(xx, yy+1.0, 0.0);

    glNormal3d(0, -1, 0);
    glVertex3d(xx, yy, 0.0);
    glVertex3d(xx+1.0, yy, 0.0);
    glVertex3d(xx+1.0, yy, 1.0);
    glVertex3d(xx, yy, 1.0);

    glNormal3d(0, 1, 0);
    glVertex3d(xx, yy+1.0, 0.0);
    glVertex3d(xx+1.0, yy+1.0, 0.0);
    glVertex3d(xx+1.0, yy+1.0, 1.0);
    glVertex3d(xx, yy+1.0, 1.0);

    glNormal3d(-1, 0, 0);
    glVertex3d(xx, yy, 0.0);
    glVertex3d(xx, yy+1.0, 0.0);
    glVertex3d(xx, yy+1.0, 1.0);
    glVertex3d(xx, yy, 1.0);

    glNormal3d(1, 0, 0);
    glVertex3d(xx+1.0, yy, 0.0);
    glVertex3d(xx+1.0, yy+1.0, 0.0);
    glVertex3d(xx+1.0, yy+1.0, 1.0);
    glVertex3d(xx+1.0, yy, 1.0);
    glEnd();
}

void Cylinder::walk_gl(){
    GLUquadricObj* q = gluNewQuadric();
    gluQuadricNormals(q, GLU_SMOOTH);
    gluQuadricTexture(q, GL_TRUE);
    gluQuadricDrawStyle(q, GLU_FILL);
    gluCylinder(q,1,1,0.3,20,20);

    glTranslatef(0.0, 0.0, 0.2);
    glNormal3f( 0.0f, 1.0f, 0.0f);
    gluDisk(q, 0.0, 1, 20, 1);

    glTranslatef(0.0, 0.0, -0.2);
    gluQuadricOrientation(q, (GLenum)GLU_INSIDE);
    glNormal3f( 0.0f, -1.0f, 0.0f);
    gluDisk(q, 0.0, 1, 20, 1);
}
