#include "particle.hpp"
#include <iostream>
#include <time.h>
#include <GL/gl.h>
#include <GL/glu.h>

particle::particle(){
    double var = rand()%100/(double)100 - 1;

    alive = 1;
    age = 0;
    mx_age = (rand()%10+1)/10.0;
    pos = Vector3D(var/10,0,0);
    dir = 0;
    //equations from http://classes.soe.ucsc.edu/cmps161/Winter09/projects/galbrech/System.cpp
    spd[0] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007);
    spd[1] = ((((((5) * rand()%11) + 5)) * rand()%11) + 1) * 0.002;
    spd[2] = (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007) - (((((((2) * rand()%11) + 1)) * rand()%11) + 1) * 0.007);
}

void particle::refresh(){
    dir += dir + ((((((int)(0.5) * rand()%11) + 1)) * rand()%11) + 1);
    pos[0] = pos[0] + spd[0];
    pos[1] = pos[1] + spd[1] + 0.05;
    pos[2] = pos[2] + spd[2];

    pos = pos+spd;
    spd = spd+0.1*spd;
    age+=0.1;
    if(age>mx_age || pos[0]>0.02 || pos[1]> 0.15 || pos[2] > 0.02){
        *this = particle();
    }
}

void particle::render(){
    glTranslated(pos[0],pos[1]+1.5,pos[2]);
    glRotated(dir-90,0,0,1);
    glBegin(GL_QUADS);
    glTexCoord2d (0, 0);
    glVertex3f (-0.05, -0.05, 0);
    glVertex3f (0.05, -0.05, 0);
    glVertex3f (0.05, 0.05, 0);
    glVertex3f (-0.05, 0.05, 0);
    glEnd();
}
