#include "scene.hpp"
#include <iostream>
#include <cmath>
#include "viewer.hpp"
#include "particle.hpp"

SceneNode::SceneNode(const std::string& name)
  : m_name(name)
{
    Matrix4x4 m;
    m_trans = m;
}

SceneNode::~SceneNode()
{
}

void SceneNode::walk_gl(int picking,int h)
{
    glPushMatrix();
    glMultMatrixd(m_trans.transpose().v_);
    for(ChildList::const_iterator it=m_children.begin();it!=m_children.end();it++){
        (*it)->walk_gl(picking,h);
    }
    glPopMatrix();
}

void SceneNode::setframe(bool b)
{
    for(ChildList::const_iterator it=m_children.begin();it!=m_children.end();it++){
        (*it)->setframe(b);
    }
}

void SceneNode::rotate(char axis, double angle)
{
    Matrix4x4 m;

    double a = angle/180*M_PI;
    double c = cos(a);
    double s = sin(a);
    if(axis == 'z'){
      m[0][0] = c;
      m[0][1] = -s;
      m[1][0] = s;
      m[1][1] = c;
    }
    else if(axis == 'x'){
      m[1][1] = c;
      m[1][2] = -s;
      m[2][1] = s;
      m[2][2] = c;
    }
    else if(axis == 'y'){
      m[0][0] = c;
      m[0][2] = s;
      m[2][0] = -s;
      m[2][2] = c;
    }

    m_trans = m_trans*m;
}

void SceneNode::scale(const Vector3D& amount)
{
  Matrix4x4 m;
  m[0][0] = amount[0];
  m[1][1] = amount[1];
  m[2][2] = amount[2];

  m_trans = m_trans*m;
}

void SceneNode::translate(const Vector3D& amount)
{
  Matrix4x4 m;
  m[0][3] = amount[0];
  m[1][3] = amount[1];
  m[2][3] = amount[2];

  m_trans = m_trans*m;
}

bool SceneNode::is_joint() const
{
  return false;
}

JointNode::JointNode(const std::string& name)
  : SceneNode(name)
{
    Matrix4x4 m;
    m_trans = m;
    angle = 0;
    hand = 0;
}

JointNode::~JointNode()
{
}

void JointNode::walk_gl(int w,int h)
{
    if(m_name == "llup_jt"){
        if(!angle){
            if(w == 0){
                if(xcurr!=-20){
                    xcurr = -20;
                    rotate('x',xcurr);
                }
            }
            else if(w==1){
                if(xcurr!=20){
                    xcurr = 20;
                    rotate('x',xcurr);
                }
            }
        }
        else{
            if(angle > 100){
                dir = -1;
            }
            else if(angle<20){
                dir = 1;
            }
            rotate('x',dir*10);
            angle = angle + dir*10;
        }
    }
    else if(m_name == "rsh_jt"){
        if(!angle){
            if(h){
                if(hand)    rotate('x',120);
                else    rotate('x',-120);
                hand = !hand;
            }
            else if(w == 0 && !hand){
                if(xcurr!=-30){
                    xcurr = -30;
                    rotate('x',xcurr);
                }
            }
            else if(w==1 && !hand){
                if(xcurr!=30){
                    xcurr = 30;
                    rotate('x',xcurr);
                }
            }
        }
        else{
            if(angle == 60){
                rotate('x',0);
                rotate('x',180);
                angle = 50;
            }
        }
    }
    else if(m_name == "lsh_jt"){
        if(!angle){
            if(w == 0){
                if(xcurr!=30){
                    xcurr = 30;
                    rotate('x',xcurr);
                }
            }
            else if(w==1){
                if(xcurr!=-30){
                    xcurr = -30;
                    rotate('x',xcurr);
                }
            }
        }
        else{
            if(angle == 60){
                rotate('x',180);
                angle = 50;
            }
        }
    }
    else if(m_name == "rlup_jt"){
        if(!angle){
            if(w == 0){
                if(xcurr!=20){
                    xcurr = 20;
                    rotate('x',xcurr);
                }
            }
            else if(w==1){
                if(xcurr!=-20){
                    xcurr = -20;
                    rotate('x',xcurr);
                }
            }
        }
        else{
            if(angle > 100){
                 dir = -1;
             }
             else if(angle<20){
                 dir = 1;
             }
             rotate('x',dir*10);
             angle = angle + dir*10;
        }
    }
    glPushMatrix();
    glMultMatrixd(m_trans.transpose().v_);
    for(ChildList::const_iterator it=m_children.begin();it!=m_children.end();it++){
        (*it)->walk_gl(w,h);
    }
    glPopMatrix();
}

bool JointNode::is_joint() const
{
  return true;
}

void JointNode::setframe(bool b)
{
    if(b){
      angle = 60;
      if(m_name == "llup_jt")
          dir = 1;
      if(m_name == "rlup_jt")
          dir = -1;
    }
    else
        angle = 0;
}

void JointNode::set_joint_x(double min, double init, double max)
{
  xcurr = init;
  if(xcurr){
      ((SceneNode*)this)->rotate('x',xcurr);
  }
}

void JointNode::set_joint_y(double min, double init, double max)
{
  ycurr = init;
  if(ycurr){
      ((SceneNode*)this)->rotate('y',ycurr);
  }
}

GeometryNode::GeometryNode(const std::string& name, Primitive* primitive)
  : SceneNode(name),
    m_primitive(primitive)
{
    Matrix4x4 m;
    m_trans = m;
}

GeometryNode::~GeometryNode()
{
}

void GeometryNode::walk_gl(int picking,int h)
{
    glPushMatrix();
    glMultMatrixd(m_trans.transpose().v_);
    if(m_name == "head"){
        glPushMatrix();
        GLfloat c[4];
        c[0] = 0;
        c[1] = 0;
        c[2] = 0;
        c[3] = 1;
        glMaterialfv(GL_FRONT, GL_DIFFUSE, c);

        glLineWidth(2);
        glBegin(GL_LINES);
        glVertex3d(0.0,1.0,0.0);
        glVertex3d(0.0,1.5,0.0);

        glVertex3d(0.0,0.5,0.0);
        glVertex3d(1.0,1.5,0.0);

        glVertex3d(0.0,0.5,0.0);
        glVertex3d(-1.0,1.5,0.0);
        glEnd();
        glPopMatrix();
    }
    if(!(m_name == "roof" || m_name == "bomb"))
        m_material->apply_gl();
    m_primitive->walk_gl();
    for(ChildList::const_iterator it=m_children.begin();it!=m_children.end();it++){
        (*it)->walk_gl(picking,h);
    }
    glPopMatrix();
}
 
