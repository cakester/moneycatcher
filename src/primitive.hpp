#ifndef CS488_PRIMITIVE_HPP
#define CS488_PRIMITIVE_HPP

#include "algebra.hpp"
#include <GL/gl.h>
#include <GL/glu.h>
#include "particle.hpp"
#include <vector>

class Primitive {
public:
  virtual ~Primitive();
  virtual void walk_gl() = 0;
};

class Sphere : public Primitive {
public:
  virtual ~Sphere();
  virtual void walk_gl();
};

class Bomb : public Primitive{
public:
    Bomb();
    void newptc();
    virtual ~Bomb(){};
    virtual void walk_gl();

    std::vector<particle> p;
};

class Cube : public Primitive {
public:
    virtual ~Cube(){}
    virtual void walk_gl();
};

class Triangle : public Primitive {
public:
    Triangle(){}
    virtual ~Triangle(){glDeleteTextures(1, &texture);};
    void loadtext();
    virtual void walk_gl();

    GLuint texture;
};

class Cylinder : public Primitive {
public:
    virtual ~Cylinder(){};
    virtual void walk_gl();
};

#endif
