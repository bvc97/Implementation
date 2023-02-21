#include "transformations.h"

// Aplica uma translacao a um triangulo //

void apply_trans_t(vec3 trans, triangle* t) {
    t->A = sum_vec3(trans, t->A);
    t->B = sum_vec3(trans, t->B);
    t->C = sum_vec3(trans, t->C);
}

// Aplica uma translacao a um vetor de triangulos //

void apply_trans(vec3 trans, vector<triangle>* boat) {
    for (int i = 0; i < boat->size(); i++) {
        apply_trans_t(trans, &((*boat)[i]));
    }
}

// Aplica uma rotacao a um triangulo //
// https://elearning.uminho.pt/bbcswebdav/pid-872136-dt-content-rid-2076591_1/courses/1819.8506N7_2/geometricTransformMaster.pdf figura 103 //

void apply_rot_t(vec3 rot, triangle* t, vec3 pos_boat) {
    TYPE d = norm_vec3(rot); // d e o angulo
    TYPE c = cos(d);
    TYPE s = sin(d);

    TYPE x = (d == 0 ? 0 : rot.x / d);
    TYPE y = (d == 0 ? 0 : rot.y / d);
    TYPE z = (d == 0 ? 0 : rot.z / d);

    TYPE xx = x * x;
    TYPE xy = x * y;
    TYPE xz = x * z;
    TYPE yy = y * y;
    TYPE yz = y * z;
    TYPE zz = z * z;

    mat3_3 m_rot = { {xx + (1 - xx) * c, xy * (1 - c) - z * s, xz * (1 - c) + y * s},
                     {xy * (1 - c) + z * s, yy + (1 - yy) * c, yz * (1 - c) - x * s},
                     {xz * (1 - c) - y * s, yz * (1 - c) + x * s, zz + (1 - zz) * c} };


    apply_trans_t(scale_vec3(-1.0, pos_boat), t);

    t->A = mult_mat_vec3(m_rot, t->A);
    t->B = mult_mat_vec3(m_rot, t->B);
    t->C = mult_mat_vec3(m_rot, t->C);

    apply_trans_t(pos_boat, t);

}

// Aplica uma rotacao a um vetor de triangulos //

void apply_rot(vec3 rot, vector<triangle>* boat, vec3 pos_boat) {
    for (int i = 0; i < boat->size(); i++) {
        apply_rot_t(rot, &((*boat)[i]), pos_boat);
    }
}

// Aplica uma rotacao a um vec3 //

vec3 apply_rot_vec3(vec3 rot, vec3 v) {
    TYPE d = norm_vec3(rot); // d e o angulo
    TYPE c = cos(d);
    TYPE s = sin(d);

    TYPE x = (d == 0 ? 0 : rot.x / d);
    TYPE y = (d == 0 ? 0 : rot.y / d);
    TYPE z = (d == 0 ? 0 : rot.z / d);

    TYPE xx = x * x;
    TYPE xy = x * y;
    TYPE xz = x * z;
    TYPE yy = y * y;
    TYPE yz = y * z;
    TYPE zz = z * z;

    mat3_3 m_rot = { {xx + (1 - xx) * c, xy * (1 - c) - z * s, xz * (1 - c) + y * s},
                     {xy * (1 - c) + z * s, yy + (1 - yy) * c, yz * (1 - c) - x * s},
                     {xz * (1 - c) - y * s, yz * (1 - c) + x * s, zz + (1 - zz) * c} };

    return mult_mat_vec3(m_rot, v);
}