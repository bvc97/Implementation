#include "boat.h"

#define VOLUME_BOAT 15.091
#define VOLUME_BOAT_2 12.366
#define VOLUME_BANHEIRA 21.296589
#define VOLUME_BIG_BOAT 2733.418945
#define VOLUME VOLUME_BOAT_2

#define MASS 7.0

// Inercia dos barcos exemplo //
#define INV_BOAT_INERTIA {{VOLUME_BOAT/(57.078*MASS),0,0},{0,VOLUME_BOAT/(58.561*MASS),0},{0,0,VOLUME_BOAT/(5.911*MASS)}}
#define INV_BOAT_2_INERTIA {{VOLUME_BOAT_2/(36.029*MASS),0,0}, {0,VOLUME_BOAT_2/(36.826*MASS), 0},{ 0,0,VOLUME_BOAT_2/(4.343*MASS)}}
#define INV_BANHEIRA_INERTIA {{VOLUME_BANHEIRA/(31.801229*MASS),0,0}, {0,VOLUME_BANHEIRA/(183.816803*MASS), 0}, {0,0,VOLUME_BANHEIRA/(166.950500*MASS)}}
#define INV_BIG_BOAT {{VOLUME_BIG_BOAT/(291130.312500*MASS),0,0},{0,VOLUME_BIG_BOAT/(297568.937500*MASS),0},{0,0,VOLUME_BIG_BOAT/(35098.875000*MASS)}}

// Inercia de primitivas //
#define INV_BOX_INERTIA(x,y,z) {{12.0/(MASS * ((y)*(y) + (z)*(z))),0,0},{0,12.0/(MASS * ((x)*(x) + (z)*(z))),0},{0,0,12.0/(MASS * ((x)*(x) + (y)*(y)))}}
#define INV_SPHERE_INERTIA(r) {{5.0/(2.0 * MASS * (r) * (r)),0,0},{0,5.0/(2.0 * MASS * (r) * (r)),0},{0,0,5.0/(2.0 * MASS * (r) * (r))}}
#define INV_TORUS_INERTIA(r,R) {{4.0/(MASS * (4*(R)*(R) + 3*(r)*(r))),0,0},{0,8.0/(MASS*(5*(r)*(r) + 4*(R)*(R))),0},{0,0,4.0/(MASS * (4*(R)*(R) + 3*(r)*(r)))}}

#define INV_INERTIA INV_BOAT_2_INERTIA //INV_BOX_INERTIA(2,2,8)

// Constantes de damping //
#define CDAMP_B 0.0
#define CDAMP_T 0.0

// Default Constructor //
Boat::Boat() {
    this->ATotal = 0;
    this->pos = { 0.0, 0.0, 0.0 };
    this->v = { 0.0, 0.0, 0.0 };
    this->theta = { 0.0, 0.0, 0.0 };
    this->w = { 0.0, 0.0, 0.0 };
    this->mass = MASS;
    this->volume = VOLUME;
    this->F = { 0.0, 0.0, 0.0 };
    this->T = { 0.0, 0.0, 0.0 };
    this->inv_inertia = INV_INERTIA;
    this->cdamp_b = CDAMP_B;
    this->cdamp_t = CDAMP_T;
}