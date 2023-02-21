#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

#include <GL/glew.h>

#include <GL/glut.h>
#include <vsl/vslibs.h> 


#define _USE_MATH_DEFINES
#include <math.h>

#include "config.h"
#include "forces.h"
#include "algorithm.h"
#include "structs.h"
#include "drawings.h"
#include "transformations.h"
#include "Grid.h"

static vec3 pos_cam = scale_vec3(10.0, { 1.0,1.0,1.0 });
static TYPE r = norm_vec3(pos_cam);
static TYPE a = M_PI / 2 - atan(pos_cam.z / pos_cam.x);
static TYPE b = M_PI / 2 - acos(pos_cam.y / r);

int timebase = 0, frame = 0;

TYPE old_time;

//plane pl = { 0.0, 1.0, 0.0, 0.0 };

/*
vector<triangle> boat = createTriangle();

vector<triangle> boat = createBox(2, 2, 2, 4);

vector<triangle> boat = createTorus(1, 3, 20, 20);

vector<triangle> boat = createSphere(3, 10, 10);
*/

Boat boat = Boat();

VSModelLib model;
VSMathLib* vsml;
VSShaderLib program;

#if (__VSL_FONT_LOADING__ == 1) && (__VSL_TEXTURE_LOADING__ == 1)
VSFontLib vsfl;
#endif

GLuint fbo;
GLuint texFBO[2];
float* pixels = NULL;

unsigned int aSentence, profileSentence;

vec3 gravity = { 0.0, -G, 0.0 };

FILE* f;

void frames() {
    TYPE fps;
    int time;
    char st[64];

    frame++;
    time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        fps = frame * 1000.0 / (time - timebase);
        timebase = time;
        frame = 0;
        sprintf(st, "FPS: %f6.2", fps);
        glutSetWindowTitle(st);
    }
}

void axis() {
    glBegin(GL_LINES);
    {
        glColor3f(1, 0, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(500, 0, 0);

        glColor3f(0, 1, 0);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 500, 0);

        glColor3f(0, 0, 1);
        glVertex3f(0, 0, 0);
        glVertex3f(0, 0, 500);
    }
    glEnd();
}

void changeSize(int w, int h) {

    // Prevent a divide by zero, when window is too short
    // (you cant make a window with zero width).
    if (h == 0)
        h = 1;

    // compute window's aspect ratio 
    TYPE ratio = w * 1.0 / h;

    // Set the projection matrix as current
    glMatrixMode(GL_PROJECTION);
    // Load Identity Matrix
    glLoadIdentity();

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // Set perspective
    gluPerspective(45.0f, ratio, 1.0f, 1000.0f);

    // return to the model view matrix mode
    glMatrixMode(GL_MODELVIEW);
}

void drawScene(TYPE new_time) {
    Grid water = Grid(old_time);

    water.draw();

    vector<triangle> notSubmerged;

    vector<triangle> submerged = part_points(boat.points, water, &notSubmerged);


    TYPE ASubmerged = total_area(submerged);

    TYPE ATotal = total_area(boat.points);

    TYPE r_n = reynolds_number(boat.v, ASubmerged, volume_submerged(submerged, boat.pos));

    vector<vec3> auxForces1 = buoyancy_forces(submerged, water);

    vector<vec3> auxForces2 = viscous_forces(boat, submerged, resistance_coefficient(r_n));

    vector<vec3> auxForces3 = drag_forces(boat, submerged);

    TYPE cos_theta_i;

    vector<TYPE> auxForces4 = slamming_areas(boat, submerged, &cos_theta_i);

    boat.F = buoyancy_force(auxForces1);

    vec3 v_force = viscous_force(auxForces2);

    vec3 d_force = drag_force(auxForces3);

    vec3 s_force = slamming_force(boat, auxForces4);

    /*
    for (int i = 0; i < auxForces1.size(); i++) {
        fprintf(f, "BuoyancyForce: %f, %f, %f; ((%f, %f, %f), (%f, %f, %f), (%f, %f, %f)) \n", auxForces1[i].x, auxForces1[i].y, auxForces1[i].z, submerged[i].A.x, submerged[i].A.y, submerged[i].A.z, submerged[i].B.x, submerged[i].B.y, submerged[i].B.z, submerged[i].C.x, submerged[i].C.y, submerged[i].C.z);
    }
    */
    
    


    vec3 boat_weight = scale_vec3(boat.mass, gravity);

    boat.F = sum_vec3(boat.F, boat_weight);

    boat.F = sum_vec3(boat.F, v_force);

    boat.F = sum_vec3(boat.F, d_force);

    boat.F = sum_vec3(boat.F, s_force);

    vector<vec3> aux_torques = torques(boat, submerged, water, boat.pos, resistance_coefficient(r_n));

    boat.T = total_torque(aux_torques);

    //fprintf(f, "Torque: %f, %f, %f\n\n", boat.T.x, boat.T.y, boat.T.z);

    //vec3 damp_force_T = damping_force(ASubmerged, ATotal, boat.w, boat.cdamp_t);

    //vec3 damp_force_B = damping_force(ASubmerged, ATotal, boat.v, boat.cdamp_b);

    //boat.F = sum_vec3(boat.F, damp_force_B);    

    //vector<TYPE> areasT = areas(submerged);

    /*
    for (int i = 0; i < areasT.size(); i++) {
        fprintf(f, "Area: %f; ((%f, %f, %f), (%f, %f, %f), (%f, %f, %f)) \n", areasT[i], submerged[i].A.x, submerged[i].A.y, submerged[i].A.z, submerged[i].B.x, submerged[i].B.y, submerged[i].B.z, submerged[i].C.x, submerged[i].C.y, submerged[i].C.z);
    }
    */



    glColor3f(1, 1, 1);
    draw(notSubmerged);
    glColor3f(0, 0.4, 1);
    draw(submerged);

    TYPE delta_t = new_time - old_time;

    vec3 new_v_boat;

    vec3 new_pos_boat = next_dynamic_t_forces(boat, boat.F, delta_t, &new_v_boat);

    apply_trans(diff_vec3(new_pos_boat, boat.pos), &boat.points);

    
    vec3 new_w_boat;

    vec3 new_theta_boat = next_dynamic_a_forces(boat, boat.T, delta_t, &new_w_boat);

    apply_rot(diff_vec3(new_theta_boat, boat.theta), &boat.points, boat.pos);
    
    

    //apply_rot({0,0.1,0}, &boat, pos_boat);

    //pos_cam = sum_vec3(pos_cam, diff_vec3(new_pos_boat, boat.pos));

    boat.pos = new_pos_boat;

    boat.v = new_v_boat;

    boat.theta = new_theta_boat;

    boat.w = new_w_boat;
}

void renderScene(void) {
    
    if (old_time - (1000 * floor(old_time / 1000))) {
        // clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set the camera
        glLoadIdentity();
        gluLookAt(pos_cam.x, pos_cam.y, pos_cam.z,
            0.0, 0.0, 0.0,
            0.0f, 1.0f, 0.0f);

        glPushMatrix();
        axis();
        glPopMatrix();

        TYPE new_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

        drawScene(new_time);

        frames();


        //TYPE new_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        // End of frame
        old_time = new_time;
        glutPostRedisplay();
        glutSwapBuffers();
    }


}


void processKeys(unsigned char c, int xx, int yy) {

    // put code to process regular keys in here
    switch (c) {
    case 'w': {
        b = b + M_PI / 12;
    }
            break;
    case 's': {
        b = b - M_PI / 12;
    }
            break;
    case 'a': {
        a = a - M_PI / 12;
    }
            break;
    case 'd': {
        a = a + M_PI / 12;
    }
            break;
    case 'x': {
        r = r - M_PI / 12;
    }
            break;
    case 'z': {
        r = r + M_PI / 12;
    }
            break;
    }

    pos_cam.x = r * cos(b) * sin(a);
    pos_cam.y = r * sin(b);
    pos_cam.z = r * cos(b) * cos(a);

    glutPostRedisplay();
}


void processSpecialKeys(int key, int xx, int yy) {

    // put code to process special keys in here

}

int init() {

    if (model.load("../../../models/boat2.obj")) {

        old_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        for (VSModelLib::MyMesh mesh : model.mMyMeshes) {

            int numIndices = mesh.numIndices;

            for (int i = 0; i < mesh.faceArray.size() / 3; i++) {

                vec3 p1 = { mesh.positions[mesh.faceArray[i * 3] * 4], mesh.positions[mesh.faceArray[i * 3] * 4 + 1], mesh.positions[mesh.faceArray[i * 3] * 4 + 2] };
                vec3 p2 = { mesh.positions[mesh.faceArray[i * 3 + 1] * 4], mesh.positions[mesh.faceArray[i * 3 + 1] * 4 + 1], mesh.positions[mesh.faceArray[i * 3 + 1] * 4 + 2] };
                vec3 p3 = { mesh.positions[mesh.faceArray[i * 3 + 2] * 4], mesh.positions[mesh.faceArray[i * 3 + 2] * 4 + 1], mesh.positions[mesh.faceArray[i * 3 + 2] * 4 + 2] };

                triangle t = { p1,p2,p3 };

                boat.points.push_back(t);


            }

        }


        //apply_rot({ 0,0,M_PI * 0.5 }, &boat.points, boat.pos);

        //apply_trans({ 0,10.0,0 }, &boat.points);

        //boat.pos = { 0,10.0,0 };

        // some GL settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT, GL_LINE);
        glEnable(GL_MULTISAMPLE);



        return true;

    }
    else {
        printf("%s\n", model.getErrors().c_str());
        return false;
    }




    /*
    old_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;


    //boat.points = createSphere(3, 10, 10);

    //boat.points = createTorus(1, 5, 10, 10);

    //boat.points = createPrism2();

    boat.points = createBox(2, 2, 2, 1);

    //apply_rot({ 0,0,M_PI*0.5 }, &boat.points, boat.pos);

    //apply_trans({ 0,10.0,0 }, &boat.points);

    //boat.points = create_tupperware();

    //boat.pos = { 0,10.0,0 };

    //boat.points = createPrism();

    //boat.points = createTriangle();


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    glEnable(GL_MULTISAMPLE);


    return true;
    */




}

int main(int argc, char** argv) {

    f = fopen("../../debug.txt", "w");

    // init GLUT and the window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA | GLUT_MULTISAMPLE);

    glutInitWindowPosition(100, 100);
    glutInitWindowSize(512, 512);
    glutCreateWindow("TRIANGLES");

    // Required callback registry 
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    // Callback registration for keyboard processing
    glutKeyboardFunc(processKeys);
    glutSpecialFunc(processSpecialKeys);

    //	Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    if (!glewIsSupported("GL_VERSION_3_3")) {
        printf("OpenGL 3.3 not supported\n");
        exit(1);
    }

    // init OpenGL and load model
    if (!init()) {
        printf("Could not Load the Model\n");
        exit(1);
    }

    glutMainLoop();

    return 1;
}