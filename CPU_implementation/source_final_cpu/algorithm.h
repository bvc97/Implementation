#ifndef TRIANGULOS_ALGORITHM_H
#define TRIANGULOS_ALGORITHM_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "forces.h"
#include "structs.h"
#include "calculos.h"
#include "triangles.h"
#include "Grid.h"

triangle_ord H_M_L(triangle t, plane pl);

bool comp_hand(triangle t, triangle s);

vector<triangle> submerged_triangle(triangle tr, Grid water, vector<triangle>* notSubmerged);

vector<triangle> part_points(vector<triangle> points, Grid water, vector<triangle>* notSubmerged);

#endif