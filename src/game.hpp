#ifndef CS488_GAME_HPP
#define CS488_GAME_HPP

#include <list>
#include "algebra.hpp"
#include "./fmod/api/inc/fmod.hpp"

class AppWindow;
class character;
class SceneNode;
class GeometryNode;
class Viewer;
class Game;

class dropping{
public:
    dropping(){};
    dropping(double);
    virtual ~dropping();
    virtual bool isbomb(){
        return false;
    }
    int hit(double pox,bool h,int dir);  //detect collision, 0 means not hit, 1 means hit pc, 2 means hit ground
    void drop();    //drop down a little
    void render();

    GeometryNode* scene;
    Matrix4x4 m;
};

class bomb:public dropping{
public:
    bomb(double);
    virtual bool isbomb(){
        return true;
    }
};

class character{
public:
    character();
    virtual ~character(){};
    void render(bool);
    void reset();
    bool move(int d);

    SceneNode* scene;
    int dir;
    Matrix4x4 m;
    Game* game;
    bool w;
    int hand;
    bool turned;
};

class Game
{
public:
    Game();
    ~Game();
    void reset();
    int tick();
    bool drop(int);
    void render();
    dropping* makedrop();
    void sound();
    void deletedrop();
    void renderfinal();
    void animation();

    bool stop;
    typedef std::list<dropping*> droppings;
    droppings drops;
    character pc;
    int score;
    Viewer* view;

    FMOD::System *system;
    FMOD::Sound *sound1, *sound2, *sound3, *sound4;
    FMOD::Channel *channel;
    bool anm;
    bool handon;
};

#endif // CS488_GAME_HPP
