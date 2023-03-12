#include "calculos.h"

// Faz a diferenca entre dois vetores //

vec3 diff_vec3(vec3 a, vec3 b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

// Faz o cross product de dois vetores //

vec3 cross_vec3(vec3 a, vec3 b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

// Faz o produto de um vetor por um escalar //

vec3 scale_vec3(NTYPE t, vec3 a) {
    return { t * a.x, t * a.y, t * a.z };
}

// Calcula a norma de um vetor //

NTYPE norm_vec3(vec3 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

// Imprime um vetor na consola apartir da funcao printf //

void print_vec3(vec3 a) {
    printf("%.20f : %.20f : %.20f\n", a.x, a.y, a.z);
}

void print_vec4(vec4 a) {
    printf("%.20f : %.20f : %.20f : %.20f\n", a.x, a.y, a.z, a.w);
}