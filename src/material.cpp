#include "material.hpp"
#include <GL/gl.h>
#include <GL/glu.h>

Material::~Material()
{
}

PhongMaterial::PhongMaterial(const Colour& kd, const Colour& ks, double shininess)
  : m_kd(kd), m_ks(ks), m_shininess(shininess)
{
}

PhongMaterial::~PhongMaterial()
{
}

void PhongMaterial::apply_gl() const
{
    GLfloat c[4];
    c[0] = m_kd.R();
    c[1] = m_kd.G();
    c[2] = m_kd.B();
    c[3] = 0.5;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, c);
}


