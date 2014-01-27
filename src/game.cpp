#include <algorithm>
#include <iostream>
#include "game.hpp"
#include "appwindow.hpp"
#include "viewer.hpp"
#include <gtkmm.h>
#include <gtkglmm.h>
#include <cstdlib>
#include <time.h>
#include <string>
#include <sstream>

const double pc_y = -0.275;
const double ground = -0.6;
const double pc_width = 0.06;
const double roof_h = 0;
const double roof_b = -0.53;
const double roof_t = roof_h + 0.39;
const double handw = 0.04;

Matrix4x4 rotation(double angle, char axis)
{
  Matrix4x4 r;

  angle = angle/180*M_PI;
  double c = cos(angle);
  double s = sin(angle);

  if(axis == 'z'){
          r[0][0] = c;
          r[0][1] = -s;
          r[1][0] = s;
          r[1][1] = c;
  }
  else if(axis == 'x'){
          r[1][1] = c;
          r[1][2] = -s;
          r[2][1] = s;
          r[2][3] = c;
  }
  else{ // y
      r[0][0] = c;
      r[0][2] = s;
      r[2][0] = -s;
      r[2][2] = c;
  }
  return r;
}

// Return a matrix to represent a displacement of the given vector.
Matrix4x4 translation(const Vector3D& displacement)
{
  Matrix4x4 t;

  t[0][3] = displacement[0];
  t[1][3] = displacement[1];
  t[2][3] = displacement[2];

  return t;
}

// Return a matrix to represent a nonuniform scale with the given factors.
Matrix4x4 scaling(const Vector3D& scale)
{
  Matrix4x4 s;

  s[0][0] = scale[0];
  s[1][1] = scale[1];
  s[2][2] = scale[2];
  return s;
}

Matrix4x4 shadow(Vector4D normal){
    Vector4D light(50,100,200,1);

    double d = normal.dot(light);

    Matrix4x4 result;

    result[0][0] = d - light[0]*normal[0];
    result[0][1] = -light[0]*normal[1];
    result[0][2] = -light[0]*normal[2];
    result[0][3] = -light[0]*normal[3];

    result[1][0] = -light[1]*normal[0];
    result[1][1] = d - light[1]*normal[1];
    result[1][2] = -light[1]*normal[2];
    result[1][3] = -light[1]*normal[3];

    result[2][0] = -light[2]*normal[0];
    result[2][1] = -light[2]*normal[1];
    result[2][2] = d - light[2]*normal[2];
    result[2][3] = -light[2]*normal[3];

    result[3][0] = -light[3]*normal[0];
    result[3][1] = -light[3]*normal[1];
    result[3][2] = -light[3]*normal[2];
    result[3][3] = d - light[3]*normal[3];

    return result;
}

Game::Game()
{
    pc = character();
    srand(time(NULL));
    stop = 0;
    score = 0;
    pc.game = this;
    anm = 0;

    sound();
}

void Game::sound(){
    FMOD::System_Create(&system);
    system->setDSPBufferSize(1024,4); 
    system->init(32, FMOD_INIT_NORMAL, 0);
    system->createSound("song.wav", FMOD_HARDWARE, 0, &sound1);
    sound1->setMode(FMOD_LOOP_NORMAL);
    system->createSound("Grenade.wav", FMOD_HARDWARE, 0, &sound2);
    system->createSound("cash.wav", FMOD_HARDWARE, 0, &sound3);
    system->createSound("scream.wav", FMOD_HARDWARE, 0, &sound4);
    system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel);
}

void Game::reset()
{
    pc.scene->setframe(0);
    channel->stop();
    deletedrop();
    pc.reset();
    srand(time(NULL));
    stop = 0;
    anm = 0;
    score = 0;
    system->playSound(FMOD_CHANNEL_FREE, sound1, false, &channel);
}

Game::~Game()
{
    deletedrop();
    sound1->release();
    sound2->release();
    sound3->release();
    sound4->release();
    system->close();
    system->release();
}

dropping::dropping(double xv){
    scene = new GeometryNode("coin",new Cylinder());
    scene->scale(Vector3D(0.04,0.04,0.04));
    scene->translate(Vector3D(xv,0,27));
    scene->rotate('x',30);
    scene->m_material = new PhongMaterial(Colour(0.8*0.1,0.5*0.1,0.2*0.1),Colour(0.4,0.4,0.4),0.5);
    m=Matrix4x4();
    m = m*translation(Vector3D(0,0.7,0));
}

dropping::~dropping(){
    delete scene;
}

void dropping::drop(){
    m = m*translation(Vector3D(0,-0.03,0));
}

void dropping::render(){
    scene->rotate('x',10);
    glPushMatrix();
    glMultMatrixd(m.transpose().v_);
    scene->walk_gl();
    glPopMatrix();
}

int dropping::hit(double pos, bool hand, int dir){
    double y = m[1][3];
    double x = scene->m_trans[0][3];
    if(y<ground){
        return 2;
    }
    else if(y<pc_y){
        if(hand){
            if(dir>0){
                if(x<=pos+pc_width+handw && x>=pos-pc_width){
                    return 1;
                }
            }
            else{
                if(x<=pos+pc_width && x>=pos-pc_width-handw){
                    return 1;
                }
            }
        }
        else{
            if(x<=pos+pc_width && x>=pos-pc_width){
                return 1;
            }
        }
    }
    return 0;
}

void character::reset(){
    dir = 1;
    std::string pcf = "pc.lua";
    SceneNode* pc = import_lua(pcf);
    scene = pc;
    m=Matrix4x4();
    w = 0;
    turned = 0;
    hand = 0;
}

bomb::bomb(double a){
    scene = new GeometryNode("bomb",new Bomb());
    scene->scale(Vector3D(0.04,0.04,0.04));
    scene->translate(Vector3D(a,0,27));
    scene->rotate('x',-20);
    scene->rotate('z',-30);
    scene->m_material = new PhongMaterial(Colour(0.01,0.01,0.01),Colour(0.4,0.4,0.4),0.5);
    m=Matrix4x4();
    m = m*translation(Vector3D(0,0.7,0));
}

character::character(){
    m=Matrix4x4();
    dir = 1;
    w = 0;
    turned = 0;
    hand = 0;
}

void character::render(bool t){
    glPushMatrix();
    glMultMatrixd(m.transpose().v_);
    if(t)
        scene->walk_gl(w,hand);
    else
        scene->walk_gl(-1,hand);

    hand = 0;
    glPopMatrix();
}

bool character::move(int d){
    //check bound
    double x = m[0][3];
    w = !w;
    if(d>0){
        if(x<0.55)
            m = m*translation(Vector3D(d*0.1,0,0));
        else{
            return 0;
        }
    }
    if(d<0){
        if(x>-0.55)
            m = m*translation(Vector3D(d*0.1,0,0));
        else{
            return 0;
        }
    }
    if(d!=dir){
        scene->rotate('y',180);
    }
    dir = d;
    turned = 1;

    return 1;
}

dropping* Game::makedrop(){
    double r = rand()%10;
    double x = rand()%300;
    dropping* d;
    x = x/10 - 15;
    if(r < 2){
        d = new bomb(x);
    }
    else{
        d = new dropping(x);
    }
    return d;
}

void Game::deletedrop(){
    while(!drops.empty()){
        dropping* d = drops.front();
        drops.pop_front();
        delete d;
    }
}

void Game::render(){
    droppings dp;
    dropping* d;

    //draw reflection
    glPushMatrix();
    glTranslated(0,0.037,2);
    glScalef(1.0,1.2,-1.0);
    if(pc.turned)
        pc.render(1);
    else
        pc.render(0);
    glPopMatrix();

    //shadow
    Vector4D gnormal(0,1,0,0.56);
    Matrix4x4 s = shadow(gnormal);
    GLdouble mx[16];

    s = s.transpose();
    for(int i=0;i<16;i++){
        mx[i] = s.v_[i];
    }

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_LIGHTING);
    glColor3f(0,0,0);
    glPushMatrix();
    glMultMatrixd(mx);
    glTranslatef(0.275,0.01,1.12);
    glScalef(1.0,0.0,1.0);
    if(pc.turned)
        pc.render(1);
    else
        pc.render(0);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    //glDisable(GL_BLEND);

    if(pc.turned)
        pc.render(1);
    else
        pc.render(0);

    pc.turned = 0;

    //shadow
    Vector4D normal(0,1,0,0.6);
    Matrix4x4 ss = shadow(normal);

    ss = ss.transpose();
    for(int i=0;i<16;i++){
        mx[i] = ss.v_[i];
    }

    GLdouble mmx[16];
    Vector4D unormal(0,0.2,1,-1);
    Matrix4x4 su = shadow(unormal);
    su = su.transpose();
    for(int i=0;i<16;i++){
        mmx[i] = su.v_[i];
    }

    while(!drops.empty()){
        d = drops.front();

        if(d->m[1][3]>roof_h && d->m[1][3]<roof_t){
            glDisable(GL_LIGHTING);
            glColor3f(0,0,0);
            glPushMatrix();
            glMultMatrixd(mmx);
            d->render();
            glPopMatrix();
            glEnable(GL_LIGHTING);
        }

        if(d->m[1][3]<roof_h && d->m[1][3]>roof_b){
            glPushMatrix();
            glTranslated(0,0.013,2);
            glScalef(1.0,1,-1.0);
            d->render();
            glPopMatrix();
        }

        if(d->m[1][3]<roof_b-0.0305){
            glDisable(GL_LIGHTING);
            glColor3f(0,0,0);
            glPushMatrix();
            glMultMatrixd(mx);
            d->render();
            glPopMatrix();
            glEnable(GL_LIGHTING);
        }

        d->render();
        dp.push_back(d);
        drops.pop_front();
    }
    while(!dp.empty()){
        d = dp.front();
        drops.push_back(d);
        dp.pop_front();
    }
}

void Game::renderfinal(){
    deletedrop();
    glDisable(GL_LIGHTING);
    glColor3f(0.1,0.1,0.1);

    //reflection
    glPushMatrix();
    glTranslated(0,0.037,2);
    glScalef(1.0,1.2,-1.0);
    if(pc.turned)
        pc.render(1);
    else
        pc.render(0);
    glPopMatrix();

    if(pc.turned)
        pc.render(1);
    else
        pc.render(0);

    //shadow
    Vector4D gnormal(0,1,0,0.6);
    Matrix4x4 s = shadow(gnormal);
    GLdouble mx[16];

    s = s.transpose();
    for(int i=0;i<16;i++){
        mx[i] = s.v_[i];
    }

    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor3f(0,0,0);
    glPushMatrix();
    glMultMatrixd(mx);
    glTranslatef(0.3,0.01,1.12);
    glScalef(1.0,0.0,1.0);
    if(pc.turned)
        pc.render(1);
    else
        pc.render(0);
    glPopMatrix();
    glEnable(GL_LIGHTING);
    //glDisable(GL_BLEND);
}

void Game::animation(){
    system->playSound(FMOD_CHANNEL_FREE, sound4, false, 0);
    if(handon){
        pc.scene->walk_gl(-1,1);
    }
    pc.scene->setframe(1);
    for(int i=0;i<4;i++){
        while(pc.move(1)){
            view->draw();
            usleep(50000);
        }
        while(pc.move(-1)){
            view->draw();
            usleep(50000);
        }
    }
    pc.m = pc.m*translation(Vector3D(-0.5,0,0));
    view->draw();
}

int Game::tick()
{
    if(!stop){
        system->update();
        // create new dropping
        int r = rand()%100;
        if(r<15){
            dropping* n = makedrop();
            drops.push_back(n);
        }

        if(drop(1)){ //game over
            stop = 1;
            view->invalidate();
            return 1;
        }

        std::ostringstream ss;
        ss<<score;
        std::string s = ss.str();
        s = "Money: " + s;
        view->lb->set_text(s.c_str());

        view->invalidate();
    }
    else{
        if(anm == 0){
            animation();
            anm = 1;
        }
        else{
            return 0;
        }
    }
    return 1;
}

bool Game::drop(int dd)
{
    droppings dp;
    dropping* d;
    while(!drops.empty()){
        d = drops.front();
        drops.pop_front();
        if(dd == 1)
            d->drop();
        int h = d->hit(pc.m[0][3],handon,pc.dir);
        if(h == 2){ // hit the ground, now we disappear
            continue;
        }
        if(h == 1){
            if(d->isbomb()){ // game over
                channel->stop();
                system->playSound(FMOD_CHANNEL_FREE, sound2, false, 0);
                return 1;
            }
            else{   // caught money
                score++;
                system->playSound(FMOD_CHANNEL_FREE, sound3, false, 0);
                continue;
            }
        }
        dp.push_back(d);
    }
    while(!dp.empty()){
        d = dp.front();
        dp.pop_front();
        drops.push_back(d);
    }
    return 0;
}
