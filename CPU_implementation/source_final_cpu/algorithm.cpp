#include "algorithm.h"

// Calcula os pontos H (high), M (medium), L (low) //

triangle_ord H_M_L(triangle t) {
    vec3 A = t.A;
    vec3 B = t.B;
    vec3 C = t.C;

    triangle_ord to;
    to.t = t;

    TYPE apply_A = A.y;
    TYPE apply_B = B.y;
    TYPE apply_C = C.y;

    if (apply_A < apply_B) {
        if (apply_A < apply_C) {
            if (apply_B < apply_C) {
                to.HML = { C,B,A };
            }
            else {
                to.HML = { B,C,A };
            }
        }
        else {
            to.HML = { B,A,C };
        }
    }
    else {
        if (apply_A > apply_C) {
            if (apply_B > apply_C) {
                to.HML = { A,B,C };
            }
            else {
                to.HML = { A,C,B };
            }
        }
        else {
            to.HML = { C,A,B };
        }
    }
    return to;
}

// Verifica se a regra da mao direita e mantida //

bool comp_hand(triangle t, triangle s) {
    return equal_sign_vec3(cross_vec3(diff_vec3(t.B, t.A), diff_vec3(t.C, t.A)), cross_vec3(diff_vec3(s.B, s.A), diff_vec3(s.C, s.A)));
}

//// Algoritmo de Corte - corta cada triangulo semi-submerso em triangulos mais pequenos totalmente submersos ////

vector<triangle> submerged_triangle(triangle tr, Grid water, vector<triangle>* notSubmerged) {
    vector<triangle> submerged;

    vec3 A = tr.A;
    vec3 B = tr.B;
    vec3 C = tr.C;

    int is_A_under = water.dist_surface(A) < 0;
    int is_B_under = water.dist_surface(B) < 0;
    int is_C_under = water.dist_surface(C) < 0;

    //vec3 H, M, L;

    triangle t, t1, t2, t3;

    

    switch (is_A_under + is_B_under + is_C_under) {
    case 0: {
        t = { A, B, C };
        notSubmerged->push_back(t);
    }
          break;

    case 1: {
        vec3 L = is_A_under ? A : is_B_under ? B : C;
        vec3 M = is_A_under ? (B.y < C.y ? B : C) : is_B_under ? (A.y < C.y ? A : C) : (A.y < B.y ? A : B);
        vec3 H = is_A_under ? (B.y >= C.y ? B : C) : is_B_under ? (A.y >= C.y ? A : C) : (A.y >= B.y ? A : B);

        TYPE hH = water.dist_surface(H);
        TYPE hM = water.dist_surface(M);
        TYPE hL = water.dist_surface(L);

        TYPE tH = (-hL) / (hH - hL);
        vec3 JH = sum_vec3(L, scale_vec3(tH, diff_vec3(H, L)));

        //res_intersect = intersect(M, L, pl);
        //if (!res_intersect.exists) break;
        TYPE tM = (-hL) / (hM - hL);
        vec3 JM = sum_vec3(L, scale_vec3(tM, diff_vec3(M, L)));

        if (comp_hand({ A,B,C }, { H,M,L })) {
            t1 = { H, M, JM };
            notSubmerged->push_back(t1);
            t2 = { JM, JH, H };
            notSubmerged->push_back(t2);

            t3 = { JM, L, JH };
            submerged.push_back(t3);
        }
        else {
            t1 = { M, H, JM };
            notSubmerged->push_back(t1);
            t2 = { JH, JM, H };
            notSubmerged->push_back(t2);

            t3 = { L, JM, JH };
            submerged.push_back(t3);
        }

    }
          break;

    case 2: {
        vec3 H = !is_A_under ? A : !is_B_under ? B : C;
        vec3 L = !is_A_under ? (B.y < C.y ? B : C) : !is_B_under ? (A.y < C.y ? A : C) : (A.y < B.y ? A : B);
        vec3 M = !is_A_under ? (B.y >= C.y ? B : C) : !is_B_under ? (A.y >= C.y ? A : C) : (A.y >= B.y ? A : B);

        TYPE hH = water.dist_surface(H);
        TYPE hM = water.dist_surface(M);
        TYPE hL = water.dist_surface(L);

        TYPE tL = (-hL) / (hH - hL);
        vec3 IL = sum_vec3(L, scale_vec3(tL, diff_vec3(H, L)));

        TYPE tM = (-hM) / (hH - hM);
        vec3 IM = sum_vec3(M, scale_vec3(tM, diff_vec3(H, M)));

        if (comp_hand({ A,B,C }, { H,M,L })) {
            t1 = { H, IM, IL };
            notSubmerged->push_back(t1);

            t2 = { IM, M, L };
            submerged.push_back(t2);
            t3 = { IM, L, IL };
            submerged.push_back(t3);
        }
        else {
            t1 = { IM, H, IL };
            notSubmerged->push_back(t1);

            t2 = { M, IM, L };
            submerged.push_back(t2);
            t3 = { L, IM, IL };
            submerged.push_back(t3);
        }

    }
          break;

    case 3: {
        t = { A, B, C };
        submerged.push_back(t);
    }
          break;

    default: break;
    }

    return submerged;

}

// Parte o conjunto de triangulos inicial em 2 conjuntos: o conjunto dos triangulos submersos e o conjunto dos triangulos nao submersos //

vector<triangle> part_points(vector<triangle> points, Grid water, vector<triangle>* notSubmerged) {
    vector<triangle> submerged;
    
    for (triangle t : points) {
        for (triangle r : submerged_triangle(t, water, notSubmerged)) {
            submerged.push_back(r);
        }
    }
    
    return submerged;
}
