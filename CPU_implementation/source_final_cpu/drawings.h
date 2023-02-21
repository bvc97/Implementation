#ifndef TRIANGULOS_DRAWINGS_H
#define TRIANGULOS_DRAWINGS_H

#include <stdio.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "structs.h"
#include "triangles.h"

void drawTriangle(triangle t);

void drawTriangle(triangle t, TYPE* A);

void draw(vector<triangle> triangles);

vector<triangle> create_tupperware();

vector<triangle> createTriangle();

vector<triangle> createPrism();

vector<triangle> createPrism2();

vector<triangle> createBox(TYPE x, TYPE y, TYPE z, int divisions);

vector<triangle> createTorus(TYPE rTubo, TYPE distAoCentro, int slices, int stacks);

vector<triangle> createSphere(TYPE radius, int slices, int stacks);

vector<triangle> createGrid(TYPE comp, TYPE larg, int divisions, plane pl);

#endif