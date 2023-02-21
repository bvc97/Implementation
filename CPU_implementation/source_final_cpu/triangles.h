#ifndef TRIANGULOS_TRIANGLES_H
#define TRIANGULOS_TRIANGLES_H

#include <stdio.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "calculos.h"
#include "structs.h"

vec3 t_center(triangle t);

vec3 normal(triangle t);

TYPE area(triangle t);

TYPE total_area(vector<triangle> ts);

TYPE volume_submerged(vector<triangle> trs, vec3 default);

vector<TYPE> areas(vector<triangle> trs);

TYPE tetra_volume(triangle t, vec3 center_of_mass);

vec3 tetra_centroid(triangle t, vec3 center_of_mass);

TYPE total_volume(vector<triangle> ts, vec3 center_of_mass);

#endif