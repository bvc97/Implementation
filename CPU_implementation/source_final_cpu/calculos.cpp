#include "calculos.h"

// Aplica a equacao do plano a um vertice //

TYPE apply(plane plano, vec3 vertice) {
    return (vertice.x * plano.a) + (vertice.y * plano.b) + (vertice.z * plano.c) + plano.d;
}

// Calcula um plano a partir de 3 pontos //

plane points_plane(vec3 A, vec3 B, vec3 C) {
    vec3 normal = normalize_vec3(cross_vec3(diff_vec3(B, A), diff_vec3(C, A)));
    TYPE d = -(normal.x * A.x + normal.y * A.y + normal.z * A.z);

    return { normal.x, normal.y, normal.z, d };
}

// Calcula a distancia entre um plano e um ponto //

TYPE distance_plp(plane pl, vec3 ponto) {
    TYPE absoluto = abs(apply(pl, ponto));

    TYPE d = norm_vec3({ pl.a,pl.b,pl.c });

    return (d == 0) ? 0 : (absoluto / d);
}

// Faz o dot product a um vetor //

TYPE dot_vec3(vec3 a, vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

// Faz a diferenca entre dois vetores //

vec3 diff_vec3(vec3 a, vec3 b) {
    return { a.x - b.x, a.y - b.y, a.z - b.z };
}

// Faz o produto de um vetor por um escalar //

vec3 scale_vec3(TYPE t, vec3 a) {
    return { t * a.x, t * a.y, t * a.z };
}

// Faz a soma de dois vetores //

vec3 sum_vec3(vec3 a, vec3 b) {
    return { a.x + b.x, a.y + b.y, a.z + b.z };
}

// Verifica se o sinal de dois vetores e igual em todas as componentes //

bool equal_sign_vec3(vec3 a, vec3 b) {
    return (a.x > 0) == (b.x > 0) && (a.y > 0) == (b.y > 0) && (a.z > 0) == (b.z > 0);
}

// Faz o cross product de dois vetores //

vec3 cross_vec3(vec3 a, vec3 b) {
    return { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
}

// Calcula o ponto de intersecao entre o vetor formado por dois pontos e o plano (e preciso ter a certeza que existe intersecao) //

maybe_vec3 intersect(vec3 a, vec3 b, plane pl) {
    TYPE d_norm = dot_vec3({ pl.a, pl.b, pl.c }, diff_vec3(a, b));

    maybe_vec3 ponto;

    if (d_norm == 0) {
        ponto.exists = false;
    }
    else {
        ponto.exists = true;
        TYPE t = -(dot_vec3({ pl.a, pl.b, pl.c }, b) + pl.d) / d_norm;
        ponto.v = sum_vec3(b, scale_vec3(t, diff_vec3(a, b)));
    }

    return ponto;
}

// Verifica se 2 vetores sao iguais //

bool equal_vec3(vec3 a, vec3 b) {
    return (a.x == b.x) && (a.y == b.y) && (a.z == b.z);
}

/*
vec3 intersect(vec3 a, vec3 b, plane pl) {
    TYPE d_norm = dot_vec3({ pl.a, pl.b, pl.c }, diff_vec3(a, b));

    TYPE t = -(dot_vec3({ pl.a, pl.b, pl.c }, b) + pl.d) / d_norm;
    vec3 ponto = sum_vec3(b, scale_vec3(t, diff_vec3(a, b)));

    return ponto;
}
*/

// Multiplica uma matrix por um vetor //

vec3 mult_mat_vec3(mat3_3 m, vec3 a) {
    return { dot_vec3(m.l1,a), dot_vec3(m.l2,a), dot_vec3(m.l3,a) };
}

// Multiplica dois vetores //

vec3 mult_vec3_vec3(vec3 a, vec3 b) {
    return { a.x * b.x, a.y * b.y, a.z * b.z };
}

// Calcula a norma de um vetor //

TYPE norm_vec3(vec3 a) {
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

// Normaliza um vetor //

vec3 normalize_vec3(vec3 a) {
    TYPE d = norm_vec3(a);
    TYPE s = (d == 0) ? 0 : (1.0 / norm_vec3(a));
    return scale_vec3(s, a);
}

// Calcula a distancia entre dois pontos //

TYPE distance_pp(vec3 a, vec3 b) {
    return norm_vec3(diff_vec3(b, a));
}

/*
vec3 spherical2Cartesian(TYPE r, TYPE a, TYPE b) {
    return
    { r * cos(b) * sin(a),
      r * sin(b),
      r * cos(b) * cos(a)
    };
}
*/

// Imprime um vetor na consola apartir da funcao printf //

void print_vec3(vec3 a) {
    printf("%.20f : %.20f : %.20f\n", a.x, a.y, a.z);
}