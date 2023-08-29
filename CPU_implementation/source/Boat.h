#ifndef TRIANGULOS_BOAT_H
#define TRIANGULOS_BOAT_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "structs.h"

class Boat {
public:
	vector<triangle> triangles;

    NTYPE ATotal;
    vec3 pos; // posicao global do centro de massa
    vec3 v; // velocidade da buoyancy no centro de massa
    vec3 theta; // angulo de rotacao
    vec3 w; // velocidade angular
    NTYPE mass;
    mat3_3 inv_inertia; // matriz de inversa de inercia
    float length;

    //Constructor
	Boat();
    void setPos(vec3 position);
    void setATotal(float areaT);
    void setLength(float b_length);
};

#endif