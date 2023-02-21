#ifndef TRIANGULOS_GRID_H
#define TRIANGULOS_GRID_H

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>

using namespace std;

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "structs.h"
#include "triangles.h"

class Grid {
public:
    int x_size = 37;
    int z_size = 37;
    TYPE length = 1;
    map<pair<int,int>, TYPE> height_map;

    //Constructor
    Grid(TYPE timer);

    plane plane_in(vec2 xz);

    TYPE height(vec2 xz);

    TYPE dist_surface(vec3 point);

    void draw();
};

#endif