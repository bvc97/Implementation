#ifndef TRIANGULOS_CALCULOS_H
#define TRIANGULOS_CALCULOS_H

#include <stdio.h>

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "structs.h"
#include "triangles.h"

TYPE apply(plane plano, vec3 vertice);

plane points_plane(vec3 A, vec3 B, vec3 C);

TYPE distance_plp(plane pl, vec3 ponto);

TYPE dot_vec3(vec3 a, vec3 b);

vec3 diff_vec3(vec3 a, vec3 b);

vec3 scale_vec3(TYPE t, vec3 a);

vec3 sum_vec3(vec3 a, vec3 b);

bool equal_sign_vec3(vec3 a, vec3 b);

vec3 cross_vec3(vec3 a, vec3 b);

maybe_vec3 intersect(vec3 a, vec3 b, plane pl);

bool equal_vec3(vec3 a, vec3 b);

//vec3 intersect(vec3 a, vec3 b, plane pl);

vec3 mult_mat_vec3(mat3_3 m, vec3 a);

vec3 mult_vec3_vec3(vec3 a, vec3 b);

TYPE norm_vec3(vec3 a);

vec3 normalize_vec3(vec3 a);

TYPE distance_pp(vec3 a, vec3 b);

//vec3 spherical2Cartesian(TYPE r, TYPE a, TYPE b);

void print_vec3(vec3 a);
#endif