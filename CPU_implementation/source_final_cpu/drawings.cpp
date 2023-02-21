#include "drawings.h"

// Desenha um triangulo guardando a sua area //

void drawTriangle(triangle t, TYPE* A) {
    *A = area(t);
    glBegin(GL_TRIANGLES);
    glVertex3f(t.A.x, t.A.y, t.A.z);
    glVertex3f(t.B.x, t.B.y, t.B.z);
    glVertex3f(t.C.x, t.C.y, t.C.z);
    glEnd();
}

// Desenha um triangulo //

void drawTriangle(triangle t) {
    glBegin(GL_TRIANGLES);
    glVertex3f(t.A.x, t.A.y, t.A.z);
    glVertex3f(t.B.x, t.B.y, t.B.z);
    glVertex3f(t.C.x, t.C.y, t.C.z);
    glEnd();
}

// Desenha um vetor de triangulos //

void draw(vector<triangle> triangles) {
    for (triangle t : triangles) {
        drawTriangle(t);
    }
}

//// Cria um tupperware ////
/*
t = {{-1.0, -1.0, 1.0}, {-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}};
t = {{-1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {1.0, -1.0, 1.0}};
t = {{-1.0, -1.0, 1.0}, {-1.0, 0.0, 0.0}, {-1.0, -1.0, -1.0}};
t = {{-1.0, -1.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, -1.0}};
t = {{-1.0, 0.0, 0.0}, {-1.0, -1.0, 1.0}, {-1.0, 0.0, 1.0}};
t = {{1.0, 0.0, 0.0}, {1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}};
t = {{1.0, 0.0, 0.0}, {1.0, -1.0, -1.0}, {1.0, 0.0, -1.0}};
t = {{1.0, 0.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 0.0, 0.0}};
t = {{-1.0, -1.0, -1.0}, {-1.0, 0.0, -1.0}, {1.0, -1.0, -1.0}};
t = {{1.0, -1.0, -1.0}, {-1.0, 0.0, -1.0}, {0.0, 0.0, -1.0}};
t = {{1.0, 0.0, -1.0}, {1.0, -1.0, -1.0}, {0.0, 0.0, -1.0}};
t = {{1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {-1.0, -1.0, 1.0}};
t = {{-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {-1.0, 0.0, 1.0}};
t = {{0.0, 0.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 0.0, 1.0}};
*/

vector<triangle> create_tupperware() {
    vector<triangle> points;
    triangle t;

    t = { {-1.0, -1.0, 1.0}, {-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0} };
    points.push_back(t);
    t = { {-1.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {1.0, -1.0, 1.0} };
    points.push_back(t);
    t = { {-1.0, -1.0, 1.0}, {-1.0, 0.0, 0.0}, {-1.0, -1.0, -1.0} };
    points.push_back(t);
    t = { {-1.0, -1.0, -1.0}, {-1.0, 0.0, 0.0}, {-1.0, 0.0, -1.0} };
    points.push_back(t);
    t = { {-1.0, 0.0, 0.0}, {-1.0, -1.0, 1.0}, {-1.0, 0.0, 1.0} };
    points.push_back(t);
    t = { {1.0, 0.0, 0.0}, {1.0, -1.0, 1.0}, {1.0, -1.0, -1.0} };
    points.push_back(t);
    t = { {1.0, 0.0, 0.0}, {1.0, -1.0, -1.0}, {1.0, 0.0, -1.0} };
    points.push_back(t);
    t = { {1.0, 0.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 0.0, 0.0} };
    points.push_back(t);
    t = { {-1.0, -1.0, -1.0}, {-1.0, 0.0, -1.0}, {1.0, -1.0, -1.0} };
    points.push_back(t);
    t = { {1.0, -1.0, -1.0}, {-1.0, 0.0, -1.0}, {0.0, 0.0, -1.0} };
    points.push_back(t);
    t = { {1.0, 0.0, -1.0}, {1.0, -1.0, -1.0}, {0.0, 0.0, -1.0} };
    points.push_back(t);
    t = { {1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {-1.0, -1.0, 1.0} };
    points.push_back(t);
    t = { {-1.0, -1.0, 1.0}, {0.0, 0.0, 1.0}, {-1.0, 0.0, 1.0} };
    points.push_back(t);
    t = { {0.0, 0.0, 1.0}, {1.0, -1.0, 1.0}, {1.0, 0.0, 1.0} };
    points.push_back(t);

    return points;
}


// Cria um triangulo //

vector<triangle> createTriangle() {
    vector<triangle> points;
   
    triangle t;
    //t = { {0.0, -10.0, 0.0}, {5.0, 10.0, 0.0}, {-5.0, 10.0, 0.0} };
    //points.push_back(t);
    
    t = { {0.0, 0.0, 5.0}, {5.0, 0.0, -5.0}, {-5.0, 0.0, -5.0} };
    points.push_back(t);

    //t = { {0.0, -1.0, 1.0}, {0.0, -1.0, -1.0}, {1.0, -1.0, -1.0} };
    //points.push_back(t);
    //t = { {0.0, -1.0, 1.0}, {1.0, -1.0, -1.0}, {1.0, -1.0, 1.0} };
    //points.push_back(t);

    return points;
}

// Cria um prisma //
vector<triangle> createPrism() {
    vector<triangle> points;

    TYPE z = 2.0;

    triangle t;
    
    //face de cima
    t.A = {-1.0, 1.0, z};
    t.B = {1.0, 1.0, z};
    t.C = {-1.0, 1.0, -z};

    points.push_back(t);

    t.A = {1.0, 1.0, z};
    t.B = {1.0, 1.0, -z};
    t.C = {-1.0,1.0,-z};

    points.push_back(t);
    
    //face da frente
    t.A = { -1.0, 1.0, z };
    t.B = { 0.0, -1.0, 1.0 };
    t.C = { 1.0, 1.0, z };

    points.push_back(t);

    //face de tras
    t.A = { -1.0, 1.0, -z };
    t.B = { 1.0, 1.0, -z };
    t.C = { 0.0, -1.0, -z };

    points.push_back(t);

    //face da esquerda
    t.A = { -1.0, 1.0, z };
    t.B = { -1.0, 1.0, -z };
    t.C = { 0.0, -1.0, 1.0 };

    points.push_back(t);

    t.A = { 0.0, -1.0, 1.0 };
    t.B = { -1.0, 1.0, -z };
    t.C = { 0.0, -1.0, -z };

    points.push_back(t);

    //face da direita
    t.A = { 1.0, 1.0, z };
    t.B = { 0.0, -1.0, 1.0 };
    t.C = { 1.0, 1.0, -z };

    points.push_back(t);

    t.A = { 0.0, -1.0, 1.0 };
    t.B = { 0.0, -1.0, -z };
    t.C = { 1.0, 1.0, -z };

    points.push_back(t);
    

    return points;
}

// Cria um prisma exemplo para o torque //

vector<triangle> createPrism2() {
    vector<triangle> points;

    TYPE z = 1.0, h = -1.5;

    triangle t;

    //face de cima
    t.A = { 0.0, -1.0, z };
    t.B = { 1.0, -1.0, z };
    t.C = { 1.0, -1.0, -z };

    points.push_back(t);

    t.A = { 1.0, -1.0, -z };
    t.B = { 0.0, -1.0, -z };
    t.C = { 0.0, -1.0, z };

    points.push_back(t);

    //face da frente
    t.A = { 1.0, h, z };
    t.B = { 1.0, -1.0, z };
    t.C = { 0.0, -1.0, z };

    points.push_back(t);

    //face de tras
    t.A = { 0.0, -1.0, -z };
    t.B = { 1.0, -1.0, -z };
    t.C = { 1.0, h, -z };

    points.push_back(t);

    //face da esquerda
    t.A = { 0.0, -1.0, z };
    t.B = { 0.0, -1.0, -z };
    t.C = { 1.0, h, z };

    points.push_back(t);

    t.A = { 0.0, -1.0, -z };
    t.B = { 1.0, h, -z };
    t.C = { 1.0, h, z };

    points.push_back(t);

    //face da direita
    t.A = { 1.0, -1.0, -z };
    t.B = { 1.0, -1.0, z };
    t.C = { 1.0, h, z };

    points.push_back(t);

    t.A = { 1.0, -1.0, -z };
    t.B = { 1.0, h, z };
    t.C = { 1.0, h, -z };

    points.push_back(t);


    return points;
}

// Cria uma caixa //

vector<triangle> createBox(TYPE x, TYPE y, TYPE z, int divisions) {
    vector<triangle> points;

    TYPE pi_x = -x / 2;
    TYPE pi_y = -y / 2;
    TYPE pi_z = -z / 2;

    TYPE pf_x = x / 2;
    TYPE pf_y = y / 2;
    TYPE pf_z = z / 2;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            //face de baixo
            triangle t;
            t.A = { pi_x + x / divisions * i, -y / 2, pi_z + z / divisions * (j + 1) };
            t.B = { pi_x + x / divisions * i, -y / 2, pi_z + z / divisions * j };
            t.C = { pi_x + x / divisions * (i + 1), -y / 2, pi_z + z / divisions * j };

            points.push_back(t);

            t.A = { pi_x + x / divisions * i, -y / 2, pi_z + z / divisions * (j + 1) };
            t.B = { pi_x + x / divisions * (i + 1), -y / 2, pi_z + z / divisions * j };
            t.C = { pi_x + x / divisions * (i + 1), -y / 2, pi_z + z / divisions * (j + 1) };

            points.push_back(t);

            //face de cima
            t.A = { pi_x + x / divisions * i, y / 2, pi_z + z / divisions * (j + 1) };
            t.B = { pi_x + x / divisions * (i + 1), y / 2, pi_z + z / divisions * j };
            t.C = { pi_x + x / divisions * i, y / 2, pi_z + z / divisions * j };

            points.push_back(t);

            t.A = { pi_x + x / divisions * i, y / 2, pi_z + z / divisions * (j + 1) };
            t.B = { pi_x + x / divisions * (i + 1), y / 2, pi_z + z / divisions * (j + 1) };
            t.C = { pi_x + x / divisions * (i + 1), y / 2, pi_z + z / divisions * j };

            points.push_back(t);

            //face da esquerda
            t.A = { -x / 2, pi_y + y / divisions * i, pi_z + z / divisions * (j + 1) };
            t.B = { -x / 2, pi_y + y / divisions * (i + 1), pi_z + z / divisions * j };
            t.C = { -x / 2, pi_y + y / divisions * i, pi_z + z / divisions * j };

            points.push_back(t);

            t.A = { -x / 2, pi_y + y / divisions * i, pi_z + z / divisions * (j + 1) };
            t.B = { -x / 2, pi_y + y / divisions * (i + 1), pi_z + z / divisions * (j + 1) };
            t.C = { -x / 2, pi_y + y / divisions * (i + 1), pi_z + z / divisions * j };

            points.push_back(t);

            //face da direita
            t.A = { x / 2, pi_y + y / divisions * i, pi_z + z / divisions * (j + 1) };
            t.B = { x / 2, pi_y + y / divisions * i, pi_z + z / divisions * j };
            t.C = { x / 2, pi_y + y / divisions * (i + 1), pi_z + z / divisions * j };

            points.push_back(t);

            t.A = { x / 2, pi_y + y / divisions * (i + 1), pi_z + z / divisions * j };
            t.B = { x / 2, pi_y + y / divisions * (i + 1), pi_z + z / divisions * (j + 1) };
            t.C = { x / 2, pi_y + y / divisions * i, pi_z + z / divisions * (j + 1) };

            points.push_back(t);

            //face de tras
            /*
            t.A = { pf_x - x / divisions * i, pf_y - y / divisions * (j + 1), -z / 2 };
            t.B = { pf_x - x / divisions * (i + 1), pf_y - y / divisions * j, -z / 2 };
            t.C = { pf_x - x / divisions * i, pf_y - y / divisions * j, -z / 2 };

            points.push_back(t);

            t.A = { pf_x - x / divisions * i, pf_y - y / divisions * (j + 1), -z / 2 };
            t.B = { pf_x - x / divisions * (i + 1), pf_y - y / divisions * (j + 1), -z / 2 };
            t.C = { pf_x - x / divisions * (i + 1), pf_y - y / divisions * j, -z / 2 };

            points.push_back(t);
            */
            t.A = { pi_x + x / divisions * i, pi_y + y / divisions * (j + 1), -z / 2 };
            t.B = { pi_x + x / divisions * (i + 1), pi_y + y / divisions * j, -z / 2 };
            t.C = { pi_x + x / divisions * i, pi_y + y / divisions * j, -z / 2 };

            points.push_back(t);

            t.A = { pi_x + x / divisions * i, pi_y + y / divisions * (j + 1), -z / 2 };
            t.B = { pi_x + x / divisions * (i + 1), pi_y + y / divisions * (j + 1), -z / 2 };
            t.C = { pi_x + x / divisions * (i + 1), pi_y + y / divisions * j, -z / 2 };

            points.push_back(t);

            //face da frente
            t.A = { pi_x + x / divisions * i, pi_y + y / divisions * (j + 1), z / 2 };
            t.B = { pi_x + x / divisions * i, pi_y + y / divisions * j, z / 2 };
            t.C = { pi_x + x / divisions * (i + 1), pi_y + y / divisions * j, z / 2 };

            points.push_back(t);

            t.A = { pi_x + x / divisions * i, pi_y + y / divisions * (j + 1), z / 2 };
            t.B = { pi_x + x / divisions * (i + 1), pi_y + y / divisions * j, z / 2 };
            t.C = { pi_x + x / divisions * (i + 1), pi_y + y / divisions * (j + 1), z / 2 };

            points.push_back(t);

        }
    }
    return points;

}

// Cria um torus //

vector<triangle> createTorus(TYPE rTubo, TYPE distAoCentro, int slices, int stacks) {
    vector<triangle> points;

    TYPE x, y, z;

    triangle t;

    for (int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            TYPE a = 2 * M_PI / slices * j;
            TYPE b = 2 * M_PI / stacks * i;

            TYPE a_n = 2 * M_PI / slices * (j + 1);
            TYPE b_n = 2 * M_PI / stacks * (i + 1);

            TYPE fi = (TYPE)i;
            TYPE fj = (TYPE)j;

            x = (distAoCentro + rTubo * cos(b_n)) * cos(a);
            y = rTubo * sin(b_n);
            z = (distAoCentro + rTubo * cos(b_n)) * sin(a);
            t.A = { x, y, z };

            x = (distAoCentro + rTubo * cos(b_n)) * cos(a_n);
            y = rTubo * sin(b_n);
            z = (distAoCentro + rTubo * cos(b_n)) * sin(a_n);
            t.B = { x, y, z };

            x = (distAoCentro + rTubo * cos(b)) * cos(a_n);
            y = rTubo * sin(b);
            z = (distAoCentro + rTubo * cos(b)) * sin(a_n);
            t.C = { x, y, z };

            points.push_back(t);

            x = (distAoCentro + rTubo * cos(b_n)) * cos(a);
            y = rTubo * sin(b_n);
            z = (distAoCentro + rTubo * cos(b_n)) * sin(a);
            t.A = { x,y,z };

            x = (distAoCentro + rTubo * cos(b)) * cos(a_n);
            y = rTubo * sin(b);
            z = (distAoCentro + rTubo * cos(b)) * sin(a_n);
            t.B = { x, y, z };

            x = (distAoCentro + rTubo * cos(b)) * cos(a);
            y = rTubo * sin(b);
            z = (distAoCentro + rTubo * cos(b)) * sin(a);
            t.C = { x, y, z };

            points.push_back(t);
        }
    }
    return points;
}

// Cria uma esfera //

vector<triangle> createSphere(TYPE radius, int slices, int stacks) {
    vector<triangle> points;

    TYPE x, y, z;

    triangle t;

    for (int j = 0; j < slices; j++) {
        //pontas de cima
        int i = stacks - 1;

        TYPE a = 2 * M_PI / slices * j;
        TYPE b = M_PI / stacks * i - M_PI / 2;

        TYPE a_n = 2 * M_PI / slices * (j + 1);
        TYPE b_n = M_PI / stacks * (i + 1) - M_PI / 2;

        TYPE fi = (TYPE)i;
        TYPE fj = (TYPE)j;

        x = radius * cos(b_n) * cos(a);
        y = radius * sin(b_n);
        z = radius * cos(b_n) * sin(a);
        t.A = { x,y,z };

        x = radius * cos(b) * cos(a_n);
        y = radius * sin(b);
        z = radius * cos(b) * sin(a_n);
        t.B = { x,y,z };

        x = radius * cos(b) * cos(a);
        y = radius * sin(b);
        z = radius * cos(b) * sin(a);
        t.C = { x,y,z };

        points.push_back(t);

        //pontas de baixo
        i = 0;
        fi = (TYPE)i;

        a = 2 * M_PI / slices * j;
        b = M_PI / stacks * i - M_PI / 2;

        a_n = 2 * M_PI / slices * (j + 1);
        b_n = M_PI / stacks * (i + 1) - M_PI / 2;

        x = radius * cos(b_n) * cos(a);
        y = radius * sin(b_n);
        z = radius * cos(b_n) * sin(a);
        t.A = { x,y,z };

        x = radius * cos(b_n) * cos(a_n);
        y = radius * sin(b_n);
        z = radius * cos(b_n) * sin(a_n);
        t.B = { x,y,z };

        x = radius * cos(b) * cos(a_n);
        y = radius * sin(b);
        z = radius * cos(b) * sin(a_n);
        t.C = { x,y,z };

        points.push_back(t);
    }


    for (int i = 1; i < stacks - 1; i++) {
        for (int j = 0; j < slices; j++) {
            TYPE a = 2 * M_PI / slices * j;
            TYPE b = M_PI / stacks * i - M_PI / 2;

            TYPE a_n = 2 * M_PI / slices * (j + 1);
            TYPE b_n = M_PI / stacks * (i + 1) - M_PI / 2;

            TYPE fi = (TYPE)i;
            TYPE fj = (TYPE)j;

            //caso base
            x = radius * cos(b_n) * cos(a);
            y = radius * sin(b_n);
            z = radius * cos(b_n) * sin(a);
            t.A = { x,y,z };

            x = radius * cos(b_n) * cos(a_n);
            y = radius * sin(b_n);
            z = radius * cos(b_n) * sin(a_n);
            t.B = { x,y,z };

            x = radius * cos(b) * cos(a_n);
            y = radius * sin(b);
            z = radius * cos(b) * sin(a_n);
            t.C = { x,y,z };

            points.push_back(t);

            x = radius * cos(b_n) * cos(a);
            y = radius * sin(b_n);
            z = radius * cos(b_n) * sin(a);
            t.A = { x,y,z };

            x = radius * cos(b) * cos(a_n);
            y = radius * sin(b);
            z = radius * cos(b) * sin(a_n);
            t.B = { x,y,z };

            x = radius * cos(b) * cos(a);
            y = radius * sin(b);
            z = radius * cos(b) * sin(a);
            t.C = { x,y,z };

            points.push_back(t);
        }
    }
    return points;
}

// Desenha uma grid //

vector<triangle> createGrid(TYPE comp, TYPE larg, int divisions, plane pl) {
    vector<triangle> points;

    triangle t;

    for (int i = 0; i < divisions; i++) {
        for (int j = 0; j < divisions; j++) {
            glColor3f(1, 0, 0);
            TYPE pi_x = -comp / 2;
            TYPE pi_z = -larg / 2;

            TYPE xmax = pi_x + comp / divisions * (i + 1);
            TYPE xmin = pi_x + comp / divisions * i;
            TYPE zmax = pi_z + larg / divisions * (j + 1);
            TYPE zmin = pi_z + larg / divisions * j;


            t.A = { xmax, -(pl.a * xmax + pl.c * zmin + pl.d) / pl.b, zmin };
            t.B = { xmin, -(pl.a * xmin + pl.c * zmin + pl.d) / pl.b, zmin };
            t.C = { xmin, -(pl.a * xmin + pl.c * zmax + pl.d) / pl.b, zmax };

            points.push_back(t);

            t.A = { xmin, -(pl.a * xmin + pl.c * zmax + pl.d) / pl.b, zmax };
            t.B = { xmax, -(pl.a * xmax + pl.c * zmax + pl.d) / pl.b, zmax };
            t.C = { xmax, -(pl.a * xmax + pl.c * zmin + pl.d) / pl.b, zmin };

            points.push_back(t);
        }
    }

    return points;
}

