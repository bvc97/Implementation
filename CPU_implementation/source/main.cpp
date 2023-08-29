#include <stdio.h>
#include <stdlib.h>
#include <vector>

using namespace std;

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vsl/vslibs.h> 

#define _USE_MATH_DEFINES
#include <math.h>

#include "config.h"
#include "Boat.h"
#include "calculos.h"
#include "structs.h"

#define _RENDERBUFFER 0
#define _TEXTURE 1
#define _NONE 2
#define FBOSIZE 512

#define _DEPTH _TEXTURE
#define _COLOR _TEXTURE

static vec3 pos_cam = scale_vec3(10, { 1.0,0.4,0.0 });
static NTYPE r = norm_vec3(pos_cam);
static NTYPE a = M_PI / 2 - atan(pos_cam.z / pos_cam.x);
static NTYPE b = M_PI / 2 - acos(pos_cam.y / r);

int timebase = 0, frame = 0;

NTYPE old_time, new_time, delta_t;

//Boat boat = Boat();

Boat boats[NUM_BOATS];

VSModelLib model, myPlane, grid;
VSMathLib* vsml;
VSShaderLib waves, ocean, algorithm, boat_length_program, forces_store, forces_sum, torques_sum, forces_application, forces_zero;

#if (__VSL_FONT_LOADING__ == 1) && (__VSL_TEXTURE_LOADING__ == 1)
VSFontLib vsfl;
#endif

GLuint fbo;
GLuint texFBO[2];
GLuint vertices, colors, forces, position, velocity, angular_velocity, angular_position, boats_inv_inertia, boats_mass, boats_ATotal, boats_length, torque, auxiliar;

vec4 m_data2[256];

int windowWidth, windowHeight;

size_t max_vertices_per_boat = 1 << 15;
size_t max_boats = 1 << 6;
float ATotal = 0;
float b_length = 0;

unsigned int aSentence, profileSentence;

// Query to measure the time spent on the GPU
GLuint64 startTime, stopTime;
unsigned int queryID[2];

vec3 gravity = { 0.0, -G, 0.0 };

void frames() {
    NTYPE fps;
    int time;
    char st[64];

    frame++;
    time = glutGet(GLUT_ELAPSED_TIME);
    if (time - timebase > 1000) {
        fps = frame * 1000.0 / (time - timebase);
        timebase = time;
        frame = 0;
        sprintf(st, "FPS: %f6.2    Time spent on the GPU: %f ms", fps, (stopTime - startTime) / 1000000.0);
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

float total_area(vector<triangle> ts) {
    NTYPE res = 0;

    for (triangle t : ts) {
        res += norm_vec3(cross_vec3(diff_vec3(t.B,t.A), diff_vec3(t.C,t.A))) / 2;
    }

    return res;
}

float boat_length(vector<triangle> ts) {
    float xmin = ts[0].A.x;
    float xmax = ts[0].A.x;
    float zmin = ts[0].A.z;
    float zmax = ts[0].A.z;

    for (triangle t : ts) {
        vec3 A = t.A;
        vec3 B = t.B;
        vec3 C = t.C;
        xmin = min(xmin, min(A.x, min(B.x, C.x)));
        xmax = max(xmax, max(A.x, max(B.x, C.x)));
        zmin = min(zmin, min(A.z, min(B.z, C.z)));
        zmax = max(zmax, max(A.z, max(B.z, C.z)));
    }

    float x_len = xmax - xmin;
    float z_len = zmax - zmin;

    return max(x_len, z_len);
}

void changeSize(int w, int h) {

    float ratio;
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if (h == 0)
        h = 1;

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    ratio = (1.0f * w) / h;
    vsml->loadIdentity(VSMathLib::PROJECTION);
    vsml->perspective(53.13f, ratio, 0.1f, 10000.0f);
    //vsml->ortho(-2 , 2 , -2/ratio, 2/ratio, -10, 10);
    windowWidth = w;
    windowHeight = h;
}

void drawScene() {
    // CREATE ALL NECESSARY BUFFERS
    // é preciso sincronizar os acessos a memoria com glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT) https://registry.khronos.org/OpenGL-Refpages/gl4/html/glMemoryBarrier.xhtml 

    // se o triangulo é cortado existem 9 vertices, senao, os ultimos 6 sao preenchidos a 0
    //               t      t      t
    // com corte: |v|v|v||v|v|v||v|v|v|
    //               t
    // sem corte: |v|v|v||0|0|0||0|0|0|

    glGenBuffers(1, &vertices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, vertices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vertices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &colors);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, colors);
    glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, colors);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &forces);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, forces);
    glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, forces);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &torque);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, torque);
    glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, torque);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


    /*glGenBuffers(1, &auxiliar);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, auxiliar);
    glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * max_boats * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 20, auxiliar);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/

    // Set both matrices to the identity matrix
    vsml->loadIdentity(VSMathLib::VIEW);
    vsml->loadIdentity(VSMathLib::MODEL);

    // start counting primitives
    glQueryCounter(queryID[0], GL_TIMESTAMP);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };


    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glDrawBuffers(1, attachments);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vsml->pushMatrix(VSMathLib::PROJECTION);
    vsml->loadIdentity(VSMathLib::PROJECTION);
    glViewport(0, 0, FBOSIZE, FBOSIZE);
    // set camera
    vsml->lookAt(0, 32, 0, 0, 0, 0, 0, 0, 1);
    vsml->ortho(-32, 32, -32, 32, 1, 1000);

    glUseProgram(waves.getProgramIndex());
    waves.setUniform("timer", old_time);

    vsml->pushMatrix(VSMathLib::MODEL);
    myPlane.render();
    vsml->popMatrix(VSMathLib::MODEL);

    vsml->popMatrix(VSMathLib::PROJECTION);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // clear default frame buffer
    glViewport(0, 0, windowWidth, windowHeight);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    vsml->loadIdentity(VSMathLib::VIEW);
    vsml->lookAt(pos_cam.x, pos_cam.y, pos_cam.z, 0, 0, 0, 0, 1, 0);

    // DRAW PLANE WITH TEXTURE FROM FBO

    glUseProgram(ocean.getProgramIndex());

    // render plane
    vsml->pushMatrix(VSMathLib::MODEL);
    grid.render();
    vsml->popMatrix(VSMathLib::MODEL);

    for (int i = 0; i < NUM_BOATS; i++) {
        // EXECUTE THE SHADERS THAT MODIFY THE BUFFERS
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        glUseProgram(algorithm.getProgramIndex());
        glViewport(0, 0, FBOSIZE, FBOSIZE);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        algorithm.setUniform("boat_idx", i);
        model.render();



        //glUseProgram(boat_length_program.getProgramIndex());
        //glMemoryBarrier(GL_ALL_BARRIER_BITS);
        ////chamar computer shader
        //glDispatchCompute(1, 1, 1);

        glUseProgram(forces_zero.getProgramIndex());
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        if (max_vertices_per_boat > (1 << 10)) {
            int ea = int(max_vertices_per_boat)/ (1 << 10);
            //int eb = max_vertices_per_boat / ea;
            glDispatchCompute(ea, 1, 1);
        }
        
        ////chamar computer shader
        //int ea = min(10, (int(log2(max_boats)) + int(log2(max_vertices_per_boat)) - 1));
        //int eb = (int(log2(max_boats)) + int(log2(max_vertices_per_boat)) - 1) - ea;
        //glDispatchCompute(int(pow(2,ea)), int(pow(2,eb)), 1);
        /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, forces);
        glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, torque);
        glBufferData(GL_SHADER_STORAGE_BUFFER, max_vertices_per_boat * sizeof(vec4), NULL, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/

        glUseProgram(forces_store.getProgramIndex());
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        forces_store.setUniform("boat_idx", i);
        model.render();




        glUseProgram(forces_sum.getProgramIndex());
        /*glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glDispatchCompute(1, 1, 1);*/
        //for (int j = min(10, int(log2(max_vertices_per_boat))) - 1; j >= 0; j--) {
        //    glMemoryBarrier(GL_ALL_BARRIER_BITS);
        //    //chamar computer shader
        //    glDispatchCompute(int(pow(2, j)), 1, 1);
        //}
        //for (int j = int(log2(max_vertices_per_boat)) - 1; j >= 0; j--) {
        //    int a = min(10, j);
        //    int b = j - a;
        //    glMemoryBarrier(GL_ALL_BARRIER_BITS);
        //    //chamar computer shader
        //    glDispatchCompute(int(pow(2, a)), int(pow(2, b)), 1);
        //}
        for (int j = int(log2(max_vertices_per_boat)) - 1; j >= 0; j--) {
            int a = max(j - 10, 1);
            int pow_e = pow(2, j);
            forces_sum.setUniform("pow_e", pow_e);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            //chamar computer shader
            glDispatchCompute(int(pow(2, a)), 1, 1);
            //printf("%d\n", j);
        }

        /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, forces);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, max_vertices_per_boat * sizeof(vec4), &m_data2);
        for (int i = 0; i < max_vertices_per_boat; i++) {
            print_vec4(m_data2[i]);
        }
        printf("\n");
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/


        glUseProgram(torques_sum.getProgramIndex());
        /*glMemoryBarrier(GL_ALL_BARRIER_BITS);
        glDispatchCompute(1, 1, 1);*/
        //for (int j = min(10, int(log2(max_vertices_per_boat))) - 1; j >= 0; j--) {
        //    glMemoryBarrier(GL_ALL_BARRIER_BITS);
        //    //chamar computer shader
        //    glDispatchCompute(int(pow(2, j)), 1, 1);
        //}
        //for (int j = int(log2(max_vertices_per_boat)) - 1; j >= 0; j--) {
        //    int a = min(10, j);
        //    int b = j - a;
        //    glMemoryBarrier(GL_ALL_BARRIER_BITS);
        //    //chamar computer shader
        //    glDispatchCompute(int(pow(2, a)), int(pow(2, b)), 1);
        //}
        for (int j = int(log2(max_vertices_per_boat)) - 1; j >= 0; j--) {
            int a = max(j - 10, 1);
            int pow_e = pow(2, j);
            torques_sum.setUniform("pow_e", pow_e);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            //chamar computer shader
            glDispatchCompute(int(pow(2, a)), 1, 1);
            //printf("%d\n", j);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        glViewport(0, 0, windowWidth, windowHeight);
        glUseProgram(forces_application.getProgramIndex());
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        forces_application.setUniform("boat_idx", i);
        model.render();

        //print_vec3(boats[0].pos);
        /*glBindBuffer(GL_SHADER_STORAGE_BUFFER, auxiliar);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, 4 * sizeof(vec4), &m_data2);
        for (int i = 0; i < 4; i++) {
            print_vec4(m_data2[i]);
        }
        printf("\n");
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);*/

        //printf("%d\n", int(pow(2,10)));
        //printf("%f\n", boats[0].length);

        //delta_t = 0.01;
    }

    delta_t = 0.009;
    //new_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    new_time = old_time + delta_t;
    //printf("%f\n", delta_t);
    delta_t = new_time - old_time;

    old_time = new_time;
    forces_application.setUniform("delta_t", delta_t);
    
    // stop counting primitives
    glQueryCounter(queryID[1], GL_TIMESTAMP);


}

void renderScene(void) {
    
    
    {   
        PROFILE("Frame");
        if (old_time - (1000 * floor(old_time / 1000))) {
            
            drawScene();


            frames();

            {
                PROFILE("Swap");
                glutPostRedisplay();
                glutSwapBuffers();
            }
        }
    }
    {
        PROFILE("Collect GL Queries Time");
        GLint stopTimerAvailable = 0;
        while (!stopTimerAvailable) {
            glGetQueryObjectiv(queryID[1],
                GL_QUERY_RESULT_AVAILABLE,
                &stopTimerAvailable);
        }

        // get query results
        glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
        glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);
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

void printFramebufferInfo(GLenum target, GLuint fbo) {

    glBindFramebuffer(target, fbo);

    int res;

    GLint buffer;
    int i = 0;
    do {
        glGetIntegerv(GL_DRAW_BUFFER0 + i, &buffer);

        if (buffer != GL_NONE) {

            printf("Shader Output Location %d - color attachment %d\n", i, buffer - GL_COLOR_ATTACHMENT0);

            glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &res);
            printf("\tAttachment Type: %s\n", res == GL_TEXTURE ? "Texture" : "Render Buffer");
            glGetFramebufferAttachmentParameteriv(target, buffer, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &res);
            printf("\tAttachment object name: %d\n", res);
        }
        ++i;

    } while (buffer != GL_NONE);
}

GLuint createR32FTexture(int w, int h) {

    GLuint tex;

    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F,
        w,
        h,
        0, GL_RED, GL_FLOAT,
        0);

    glBindTexture(GL_TEXTURE_2D, 0);

    return(tex);
}

GLuint createDepthTexture(int w, int h) {

    GLuint tex;

    glGenTextures(1, &tex);

    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        w,
        h,
        0, GL_DEPTH_COMPONENT, GL_FLOAT,
        NULL);

    glBindTexture(GL_TEXTURE_2D, 0);

    return(tex);
}

GLuint prepareFBO(int w, int h, int colorCount) {

    GLuint fbo = 0;
    // Generate one frame buffer
    glGenFramebuffers(1, &fbo);
    // bind it
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // attach textures for colors
    for (int i = 0; i < colorCount; ++i) {
        texFBO[i] = createR32FTexture(w, h);

#if (_COLOR == _TEXTURE)
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texFBO[i], 0);
#elif (_COLOR == _RENDERBUFFER)
        GLuint rbAux;
        glGenRenderbuffers(1, &rbAux);
        rb[i] = rbAux;
        glBindRenderbuffer(GL_RENDERBUFFER, rb[i]);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_RENDERBUFFER, rb[i]);
#endif
    }

#if (_DEPTH == _RENDERBUFFER)
    // attach renderbuffer for depth
    GLuint fb;
    glGenRenderbuffers(1, &fb);
    glBindRenderbuffer(GL_RENDERBUFFER, fb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, fb);

#elif (_DEPTH == _TEXTURE)
    // attach depth texture
    GLuint depthFBO;
    depthFBO = createDepthTexture(w, h);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthFBO, 0);
#endif
    // check if everything is OK
    GLenum e = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (e) {

    case GL_FRAMEBUFFER_UNDEFINED:
        printf("FBO Undefined\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        printf("FBO Incomplete Attachment\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        printf("FBO Missing Attachment\n");
        break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
        printf("FBO Incomplete Draw Buffer\n");
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED:
        printf("FBO Unsupported\n");
        break;
    case GL_FRAMEBUFFER_COMPLETE:
        printf("FBO OK\n");
        break;
    default:
        printf("FBO Problem?\n");
    }

    if (e != GL_FRAMEBUFFER_COMPLETE)
        return (0);
    // unbind fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //	printFramebuffersInfo(fbo);

    return fbo;
}

int init() {
    
    if (model.load("../../../models/boat4_complex.obj") 
        && myPlane.load("../../../models/grid_64.obj")
        && grid.load("../../../models/grid_64.obj")) {

        old_time = 1;//glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        delta_t = 0;
        
        for (Boat b : boats) {
            for (VSModelLib::MyMesh mesh : model.mMyMeshes) {

                int numIndices = mesh.numIndices;

                for (int i = 0; i < mesh.faceArray.size()/3; i++) {

                    vec3 p1 = { mesh.positions[mesh.faceArray[i * 3] * 4], mesh.positions[mesh.faceArray[i * 3] * 4 + 1], mesh.positions[mesh.faceArray[i * 3] * 4 + 2] };
                    vec3 p2 = { mesh.positions[mesh.faceArray[i * 3 + 1] * 4], mesh.positions[mesh.faceArray[i * 3 + 1] * 4 + 1], mesh.positions[mesh.faceArray[i * 3 + 1] * 4 + 2] };
                    vec3 p3 = { mesh.positions[mesh.faceArray[i * 3 + 2] * 4], mesh.positions[mesh.faceArray[i * 3 + 2] * 4 + 1], mesh.positions[mesh.faceArray[i * 3 + 2] * 4 + 2] };

                    triangle t = { p1,p2,p3 };

                    b.triangles.push_back(t);


                }

            }
            ATotal = total_area(b.triangles);
            b_length = boat_length(b.triangles);
            printf("%d\n", b.triangles.size());
        }
        

        

        // some GL settings
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT, GL_LINE);
        glEnable(GL_MULTISAMPLE);

        
        fbo = prepareFBO(FBOSIZE, FBOSIZE, 1);

        if (fbo == 0) {
            printf("Error in FBO creation\n");
            return false;
        }

        printFramebufferInfo(GL_DRAW_FRAMEBUFFER, fbo);
        
        
        grid.setTexture(0, texFBO[0]);
        model.setTexture(0, texFBO[0]);
        
        glGenQueries(2, queryID);
        return true;

    }
    else {
        printf("%s\n", model.getErrors().c_str());
        return false;
    }
    
    
    
    
    /*
    old_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    
    //boat.triangles = createSphere(3, 10, 10);
    
    //boat.triangles = createTorus(1, 5, 10, 10);
    
    //boat.triangles = createPrism2();
    
    boat.triangles = createBox(2, 2, 2, 1);

    //apply_rot({ 0,0,M_PI*0.5 }, &boat.triangles, boat.pos);

    //apply_trans({ 0,10.0,0 }, &boat.triangles);

    //boat.triangles = create_tupperware();

    //boat.pos = { 0,10.0,0 };

    //boat.triangles = createPrism();

    //boat.triangles = createTriangle();
    

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_LINE);
    glEnable(GL_MULTISAMPLE);
    

    return true;
    */
    
    
    
    
}

GLuint setupShaders() {
    float pos_aux[NUM_BOATS * 4];
    float vel_aux[NUM_BOATS * 4];
    float ang_vel_aux[NUM_BOATS * 4];
    float ang_pos_aux[NUM_BOATS * 4];
    float boat_inv_inertia_aux[NUM_BOATS * 16];
    float boats_mass_aux[NUM_BOATS];
    float boats_ATotal_aux[NUM_BOATS];
    float boats_length_aux[NUM_BOATS];

    //srand(time(NULL));
    //
    //for (int i = 0; i < NUM_BOATS; i++) {
    //    float pos_x = ((float)rand()/(float)RAND_MAX) * 32 - 16;
    //    float pos_z = ((float)rand()/(float)RAND_MAX) * 32 - 16;
    //    boats[i].setPos({ pos_x,0, pos_z });
    //}

    for (int i = 0; i < NUM_BOATS; i++) {
        pos_aux[i * 4] = boats[i].pos.x;
        pos_aux[i * 4 + 1] = boats[i].pos.y;
        pos_aux[i * 4 + 2] = boats[i].pos.z;
        pos_aux[i * 4 + 3] = 1;

        vel_aux[i * 4] = boats[i].v.x; 
        vel_aux[i * 4 + 1] = boats[i].v.y;
        vel_aux[i * 4 + 2] = boats[i].v.z;
        vel_aux[i * 4 + 3] = 0;

        ang_vel_aux[i * 4] = boats[i].w.x;
        ang_vel_aux[i * 4 + 1] = boats[i].w.y;
        ang_vel_aux[i * 4 + 2] = boats[i].w.z;
        ang_vel_aux[i * 4 + 3] = 0;

        ang_pos_aux[i * 4] = boats[i].theta.x;
        ang_pos_aux[i * 4 + 1] = boats[i].theta.y;
        ang_pos_aux[i * 4 + 2] = boats[i].theta.z;
        ang_pos_aux[i * 4 + 3] = 1;

        boat_inv_inertia_aux[i * 16] = boats[i].inv_inertia.l1.x;
        boat_inv_inertia_aux[i * 16 + 1] = boats[i].inv_inertia.l2.x;
        boat_inv_inertia_aux[i * 16 + 2] = boats[i].inv_inertia.l3.x;
        boat_inv_inertia_aux[i * 16 + 3] = 0;
        boat_inv_inertia_aux[i * 16 + 4] = boats[i].inv_inertia.l1.y;
        boat_inv_inertia_aux[i * 16 + 5] = boats[i].inv_inertia.l2.y;
        boat_inv_inertia_aux[i * 16 + 6] = boats[i].inv_inertia.l3.y;
        boat_inv_inertia_aux[i * 16 + 7] = 0;
        boat_inv_inertia_aux[i * 16 + 8] = boats[i].inv_inertia.l1.z;
        boat_inv_inertia_aux[i * 16 + 9] = boats[i].inv_inertia.l2.z;
        boat_inv_inertia_aux[i * 16 + 10] = boats[i].inv_inertia.l3.z;
        boat_inv_inertia_aux[i * 16 + 11] = 0;
        boat_inv_inertia_aux[i * 16 + 12] = 0;
        boat_inv_inertia_aux[i * 16 + 13] = 0;
        boat_inv_inertia_aux[i * 16 + 14] = 0;
        boat_inv_inertia_aux[i * 16 + 15] = 0;

        boats_mass_aux[i] = boats[i].mass;
        boats[i].setATotal(ATotal);
        boats_ATotal_aux[i] = boats[i].ATotal;
        boats[i].setLength(b_length);
        boats_length_aux[i] = boats[i].length;
    }
    
    glGenBuffers(1, &position);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, position);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(vec4), pos_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, position);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &velocity);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, velocity);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(vec4), vel_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, velocity);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &angular_velocity);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, angular_velocity);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(vec4), ang_vel_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, angular_velocity);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &angular_position);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, angular_position);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(vec4), ang_pos_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, angular_position);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &boats_inv_inertia);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, boats_inv_inertia);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * 4 * sizeof(vec4), boat_inv_inertia_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 9, boats_inv_inertia);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &boats_mass);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, boats_mass);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(float), boats_mass_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 10, boats_mass);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &boats_ATotal);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, boats_ATotal);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(float), boats_ATotal_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 11, boats_ATotal);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenBuffers(1, &boats_length);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, boats_length);
    glBufferData(GL_SHADER_STORAGE_BUFFER, NUM_BOATS * sizeof(float), boats_length_aux, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 12, boats_length);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    
    std::string path = PATH_TO_FILES;

    // Shader for test
    waves.init();
    waves.loadShader(VSShaderLib::VERTEX_SHADER, path + "shaders/waves.vert");
    waves.loadShader(VSShaderLib::FRAGMENT_SHADER, path + "shaders/waves.frag");

    // set semantics for the shader variables
    waves.setProgramOutput(0, "out1");
    waves.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

    waves.prepareProgram();

    VSGLInfoLib::getProgramInfo(waves.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(waves.getProgramIndex());

    // add sampler uniforms
    waves.setUniform("A", 1.0f); //0.8f //0.2f
    waves.setUniform("Q", 0.0f);
    waves.setUniform("w", 0.5f); //0.8f //2.0f
    waves.setUniform("phi", 0.2f); //1.0f
    waves.setUniform("timer", old_time);

    NTYPE d[2] = { 0.0f, 1.0f };
    waves.setUniform("D", d);

    printf("InfoLog for Model Shader\n%s\n", waves.getAllInfoLogs().c_str());


    // Shader for test
    ocean.init();
    ocean.loadShader(VSShaderLib::VERTEX_SHADER, path + "shaders/ocean.vert");
    ocean.loadShader(VSShaderLib::FRAGMENT_SHADER, path + "shaders/ocean.frag");

    // set semantics for the shader variables
    ocean.setProgramOutput(0, "out2");
    ocean.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

    ocean.prepareProgram();

    VSGLInfoLib::getProgramInfo(ocean.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(ocean.getProgramIndex());

    // add sampler uniforms
    ocean.setUniform("texUnit", 0);
    ocean.setUniform("fboSize", FBOSIZE);

    printf("InfoLog for Model Shader\n%s\n", ocean.getAllInfoLogs().c_str());

    
    // Shader for test
    algorithm.init();
    algorithm.loadShader(VSShaderLib::VERTEX_SHADER, path + "shaders/cut.vert");
    algorithm.loadShader(VSShaderLib::GEOMETRY_SHADER, path + "shaders/cut.geom");
    algorithm.loadShader(VSShaderLib::FRAGMENT_SHADER, path + "shaders/cut.frag");

    // set semantics for the shader variables
    algorithm.setProgramOutput(0, "out3");
    algorithm.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

    algorithm.prepareProgram();

    VSGLInfoLib::getProgramInfo(algorithm.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(algorithm.getProgramIndex());

    // add sampler uniforms
    algorithm.setUniform("texUnit", 0);
    algorithm.setUniform("fboSize", FBOSIZE);
    algorithm.setUniform("max_vertices_per_boat", (int)max_vertices_per_boat);

    printf("InfoLog for Model Shader\n%s\n", algorithm.getAllInfoLogs().c_str());
    
    // Shader for test
    forces_zero.init();
    forces_zero.loadShader(VSShaderLib::COMPUTE_SHADER, path + "shaders/forces_zero.comp");

    // set semantics for the shader variables
    forces_zero.prepareProgram();

    VSGLInfoLib::getProgramInfo(forces_zero.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(forces_zero.getProgramIndex());

    // add sampler uniforms

    printf("InfoLog for Model Shader\n%s\n", forces_zero.getAllInfoLogs().c_str());

    // Shader for test
    forces_store.init();
    //forces_store.loadShader(VSShaderLib::COMPUTE_SHADER, path + "shaders/forces_store.comp");
    forces_store.loadShader(VSShaderLib::VERTEX_SHADER, path + "shaders/forces_store.vert");
    forces_store.loadShader(VSShaderLib::GEOMETRY_SHADER, path + "shaders/forces_store.geom");
    forces_store.loadShader(VSShaderLib::FRAGMENT_SHADER, path + "shaders/forces_store.frag");

    // set semantics for the shader variables
    forces_store.setProgramOutput(0, "out4");
    forces_store.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

    forces_store.prepareProgram();

    VSGLInfoLib::getProgramInfo(forces_store.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(forces_store.getProgramIndex());

    // add sampler uniforms
    forces_store.setUniform("texUnit", 0);
    forces_store.setUniform("fboSize", FBOSIZE);
    /*
    forces_store.setUniform("boat_mass", boats[0].mass);
    forces_store.setUniform("boat_ATotal", boats[0].ATotal);
    forces_store.setUniform("boat_length", boats[0].length);*/
    forces_store.setUniform("max_vertices_per_boat", (int)max_vertices_per_boat);

    printf("InfoLog for Model Shader\n%s\n", forces_store.getAllInfoLogs().c_str());
    
    // Shader for test
    forces_sum.init();
    forces_sum.loadShader(VSShaderLib::COMPUTE_SHADER, path + "shaders/forces_sum.comp");

    // set semantics for the shader variables
    forces_sum.prepareProgram();

    VSGLInfoLib::getProgramInfo(forces_sum.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(forces_sum.getProgramIndex());

    // add sampler uniforms
    forces_sum.setUniform("max_vertices_per_boat", (int)max_vertices_per_boat);
    forces_sum.setUniform("max_boats", (int)max_boats);

    printf("InfoLog for Model Shader\n%s\n", forces_sum.getAllInfoLogs().c_str());

    // Shader for test
    torques_sum.init();
    torques_sum.loadShader(VSShaderLib::COMPUTE_SHADER, path + "shaders/torques_sum.comp");

    // set semantics for the shader variables
    torques_sum.prepareProgram();

    VSGLInfoLib::getProgramInfo(torques_sum.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(torques_sum.getProgramIndex());

    // add sampler uniforms
    torques_sum.setUniform("max_vertices_per_boat", (int)max_vertices_per_boat);
    torques_sum.setUniform("max_boats", (int)max_boats);

    printf("InfoLog for Model Shader\n%s\n", torques_sum.getAllInfoLogs().c_str());

    // Shader for test
    forces_application.init();
    forces_application.loadShader(VSShaderLib::VERTEX_SHADER, path + "shaders/forces_application.vert");
    forces_application.loadShader(VSShaderLib::GEOMETRY_SHADER, path + "shaders/forces_application.geom");
    forces_application.loadShader(VSShaderLib::FRAGMENT_SHADER, path + "shaders/forces_application.frag");

    // set semantics for the shader variables
    forces_application.setProgramOutput(0, "out5");
    forces_application.setVertexAttribName(VSShaderLib::VERTEX_COORD_ATTRIB, "position");

    forces_application.prepareProgram();

    VSGLInfoLib::getProgramInfo(forces_application.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(forces_application.getProgramIndex());

    // add sampler uniforms
    /*forces_application.setUniform("boat_mass", boats[0].mass);*/
    forces_application.setUniform("num_boats", NUM_BOATS);
    forces_application.setUniform("max_vertices_per_boat", (int)max_vertices_per_boat);
    //forces_application.setUniform("boat_inv_inertia", boat_inv_inertia);
    forces_application.setUniform("delta_t", delta_t);

    printf("InfoLog for Model Shader\n%s\n", forces_application.getAllInfoLogs().c_str());

    // Shader for test
    boat_length_program.init();
    boat_length_program.loadShader(VSShaderLib::COMPUTE_SHADER, path + "shaders/boat_length.comp");

    boat_length_program.prepareProgram();

    VSGLInfoLib::getProgramInfo(boat_length_program.getProgramIndex());
    VSGLInfoLib::getUniformsInfo(boat_length_program.getProgramIndex());

    printf("InfoLog for Model Shader\n%s\n", boat_length_program.getAllInfoLogs().c_str());

    return (waves.isProgramLinked() && ocean.isProgramLinked() && algorithm.isProgramLinked() && boat_length_program.isProgramLinked() && forces_store.isProgramLinked() && forces_sum.isProgramLinked() && torques_sum.isProgramLinked() && forces_application.isProgramLinked());
}

void initVSL() {
    //	 Init VSML
    vsml = VSMathLib::getInstance();
    vsml->setUniformName(VSMathLib::PROJ_VIEW_MODEL, "m_pvm");
    vsml->setUniformName(VSMathLib::PROJ_VIEW, "m_pv");
    vsml->setUniformName(VSMathLib::VIEW, "m_view");
    vsml->setUniformName(VSMathLib::NORMAL, "m_normal");
    vsml->setUniformName(VSMathLib::PROJECTION, "m_proj");
    vsml->setUniformName(VSMathLib::MODEL, "m_model");

#if (__VSL_TEXTURE_LOADING__ == 1)
    // Init VSFL Fonts
    vsfl.load("../../fonts/couriernew10");
    vsfl.setFixedFont(true);
    aSentence = vsfl.genSentence();
    profileSentence = vsfl.genSentence();
#endif
}

int main(int argc, char** argv) {
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

    //	return from main loop
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    //	Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();
    if (!glewIsSupported("GL_VERSION_4_4")) {
        printf("OpenGL 4.4 not supported\n");
        exit(1);
    }

    
    VSGLInfoLib::getGeneralInfo();

    initVSL();

    // init OpenGL and load model
    if (!init()) {
        printf("Could not Load the Model\n");
        exit(1);
    }

    if (!setupShaders())
        exit(1);

    glutMainLoop();

    return 1;
}