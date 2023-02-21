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
#include "triangles.h"

class Boat {
public:
	vector<triangle> points;

    TYPE ATotal;
    vec3 pos; // posicao global do centro de massa
    vec3 v; // velocidade da buoyancy no centro de massa
    vec3 theta; // angulo de rotacao
    vec3 w; // velocidade angular
    TYPE mass;
    TYPE volume;
    vec3 F;
    vec3 T;
    mat3_3 inv_inertia; // matriz de inversa de inercia
    TYPE cdamp_b; // constante de damping para a buoyancy
    TYPE cdamp_t; // constante de damping para o torque

    //Constructor
	Boat();
};

#endif