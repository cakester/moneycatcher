#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include "algebra.hpp"

class particle{
public:
    particle();
    void refresh();
    void render();

    Vector3D pos;
    Vector3D spd;
    double dir;
    bool alive;
    double age;
    double mx_age;
};

#endif
