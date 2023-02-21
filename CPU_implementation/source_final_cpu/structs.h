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
#define TYPE double


struct vec3 {
    TYPE x, y, z;
};

struct vec2 {
    TYPE x, z;
};

struct pair2 {
    int x, z;
};

struct plane {
    TYPE a, b, c, d;
};

struct triangle {
    vec3 A, B, C;
};

struct triangle_ord {
    triangle t;
    triangle HML;
};

struct mat3_3 {
    vec3 l1, l2, l3;
};

struct maybe_vec3 {
    bool exists;
    vec3 v;
};

/*
struct my_boat {
    vector<triangle> points;
    TYPE ATotal = 0;
    vec3 pos = { 0.0, 0.0, 0.0 };
    vec3 v = { 0.0, 0.0, 0.0 };
    vec3 theta = { 0.0, 0.0, 0.0 };
    vec3 w = { 0.0, 0.0, 0.0 };
    TYPE mass = MASS;
    mat3_3 inv_inertia = INV_INERTIA;
    TYPE cdamp_b = CDAMP_B;
    TYPE cdamp_t = CDAMP_T;
};

*/


#endif