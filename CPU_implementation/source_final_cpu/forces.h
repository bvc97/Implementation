#ifndef TRIANGULOS_FORCES_H
#define TRIANGULOS_FORCES_H

#include <stdio.h>

#include <GL/glew.h>
#include <GL/glut.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include "calculos.h"
#include "algorithm.h"
#include "Boat.h"
#include "transformations.h"
#include "Grid.h"

vec3 buoyancy_force(triangle t, Grid water);

vector<vec3> buoyancy_forces(vector<triangle> trs, Grid water);

vec3 buoyancy_force(vector<vec3> buoyancy_forces);

vec3 viscous_water_resistance(Boat boat, triangle t, TYPE Cf);

vec3 triangle_torque(triangle t, vec3 center_of_mass, vec3 force_i);

vector<vec3> torques(Boat boat, vector<triangle> trs, Grid water, vec3 center_of_mass, TYPE Cf);

vec3 total_torque(vector<vec3> torques);

vec3 damping_force(TYPE ASubmerged, TYPE ATotal, vec3 w, TYPE cdamp);

vec3 triangle_velocity(Boat boat, triangle t);

TYPE reynolds_number(vec3 v, TYPE ASubmerged, TYPE VSubmerged);

TYPE resistance_coefficient(TYPE Rn);

vec3 viscous_water_resistance(Boat boat, triangle t, TYPE Cf);

vector<vec3> viscous_forces(Boat boat, vector<triangle> trs, TYPE Cf);

vec3 viscous_force(vector<vec3> viscous_forces);

vec3 pressure_drag_forces(Boat boat, triangle t);

vector<vec3> drag_forces(Boat boat, vector<triangle> trs);

vec3 drag_force(vector<vec3> drag_forces);

TYPE submerged_triangle_area(Boat boat, triangle t, TYPE * cos_theta_i);

vector<TYPE> slamming_areas(Boat boat, vector<triangle> trs, TYPE * cos_theta_i);

vec3 slamming_force(Boat boat, vector<TYPE> slamming_areas);

vec3 next_dynamic_t_forces(Boat boat, vec3 F, TYPE d_t, vec3* new_v);

vec3 next_dynamic_a_forces(Boat boat, vec3 T, TYPE d_t, vec3* new_w);

#endif