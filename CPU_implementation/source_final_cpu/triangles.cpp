#include "triangles.h"

// Calcula o centroide de um triangulo //

vec3 t_center(triangle t) {
    vec3 s1 = sum_vec3(t.A, t.B);
    vec3 s2 = sum_vec3(s1, t.C);
    TYPE s = 1.0 / 3.0;
    vec3 s3 = scale_vec3(s, s2);
    return s3;
}

// Calcula a normal de um triangulo //

vec3 normal(triangle t) {
    vec3 U = diff_vec3(t.B, t.A);
    vec3 V = diff_vec3(t.C, t.A);

    vec3 N = cross_vec3(U, V);

    return normalize_vec3(N);
}

// Calcula a area de um triangulo //

TYPE area(triangle t) {
    return norm_vec3(cross_vec3(diff_vec3(t.B, t.A), diff_vec3(t.C, t.A))) * 0.5;
}

// Calcula a area total dos triangulos de um vetor //

TYPE total_area(vector<triangle> ts) {
    TYPE res = 0;

    for (triangle t : ts) {
        res += area(t);
    }

    return res;
}

// Calcula o volume de um objeto a partir do seu volume de triangulos e do centro de massa

TYPE volume_submerged(vector<triangle> trs, vec3 default) {
    TYPE volume_tetras = 0;
    for (triangle t : trs) {
        volume_tetras += tetra_volume(t, default);
    }
    return volume_tetras;
}

// Calcula o vetor de areas de um vetor de triangulos //

vector<TYPE> areas(vector<triangle> trs) {
    vector<TYPE> res;
    for (triangle t : trs)
        res.push_back(area(t));
    return res;
}

// Calcula o volume de um tetraedro //

TYPE tetra_volume(triangle t, vec3 center_of_mass) {
    vec3 a = diff_vec3(t.A, center_of_mass);
    vec3 b = diff_vec3(t.B, center_of_mass);
    vec3 c = diff_vec3(t.C, center_of_mass);

    return dot_vec3(a, cross_vec3(b, c)) / 6.0;
}

// Calcula o volume total de um objeto feito a partir de triangulos //

TYPE total_volume(vector<triangle> ts, vec3 center_of_mass) {
    TYPE res = 0;

    for (triangle t : ts) {
        res += tetra_volume(t, center_of_mass);
    }

    return res;
}



// Calcula o centroid de um tetraedro //

vec3 tetra_centroid(triangle t, vec3 center_of_mass) {
    vec3 apex = center_of_mass;

    return scale_vec3(1.0/4.0, sum_vec3(apex, sum_vec3(t.A, sum_vec3(t.B, t.C))));
} 
