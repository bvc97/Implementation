#ifndef TRIANGULOS_CALCULOS_H
#define TRIANGULOS_CALCULOS_H

#include <stdio.h>

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "structs.h"

vec3 diff_vec3(vec3 a, vec3 b);

vec3 cross_vec3(vec3 a, vec3 b);

vec3 scale_vec3(NTYPE t, vec3 a);

NTYPE norm_vec3(vec3 a);

void print_vec3(vec3 a);

void print_vec4(vec4 a);

#endif