#ifndef TRIANGULOS_STRUCTS_H
#define TRIANGULOS_STRUCTS_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>
 
#define G 9.8
#define DENS 1.0
#define NTYPE float
#define NUM_BOATS 1

struct vec4 {
    NTYPE x, y, z, w;
};

struct vec3 {
    NTYPE x, y, z;
};

struct triangle {
    vec3 A, B, C;
};

struct mat3_3 {
    vec3 l1, l2, l3;
};

#endif