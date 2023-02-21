#ifndef TRIANGULOS_TRANSFORMATIONS_H
#define TRIANGULOS_TRANSFORMATIONS_H

#include <stdio.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "structs.h"
#include "triangles.h"

void apply_trans_t(vec3 trans, triangle* t);

void apply_trans(vec3 trans, vector<triangle>* boat);

void apply_rot_t(vec3 rot, triangle* t, vec3 pos_boat);

void apply_rot(vec3 rot, vector<triangle>* boat, vec3 pos_boat);

vec3 apply_rot_vec3(vec3 rot, vec3 v);

#endif