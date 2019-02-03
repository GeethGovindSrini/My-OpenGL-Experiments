////////////////////////////////////////////////////////////////
// Ekalaiva.cpp
//
// This program performs arrow shooting like Ekalaiva.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press the up/down arrow keys to speed up/slow down animation.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Geeth Govind Srinivasulu.
////////////////////////////////////////////////////////////////

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <vector>

#ifdef __APPLE__
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <OpenGL/glext.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#pragma comment(lib, "glew32.lib")
#endif

#define PI 3.14159265358979324

using namespace std;

// Globals.
static int width, height;                              // OpenGL window size.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0;                              // Animated?
static int animationPeriod = 100;                      // Time interval between frames.
static float arrowPos = 300;
//Initial Position if the arrow
static float X = 0.0;
static float Y = 0.0;
static float Z = 300.0;
//Properties determining the movement of the arrow
static float t = 0.0;  // Time parameter.
static float h = 01.0; // Horizontal component of initial velocity.
static float v = 1.3;  // Vertical component of initial velocity.
static float g = 0.2;  // Gravitational accelaration.

//Circle Properties
int numVertices = 60; //Total number of vertices

//Required for Text
static char theStringBuffer[10];              // String buffer.
static long font = (long)GLUT_BITMAP_8_BY_13; // Font selection.
// Routine to draw a bitmap character string.
void writeBitmapString(void *font, char *string)
{
    char *c;

    for (c = string; *c != '\0'; c++)
        glutBitmapCharacter(font, *c);
}

// Routine to onvert floating point to char string.
void floatToString(char *destStr, int precision, float val)
{
    sprintf(destStr, "%f", val);
    destStr[precision] = '\0';
}

// Write data.
void writeData(void)
{
    glColor3f(1.0, 1.0, 1.0);

    floatToString(theStringBuffer, 4, h);
    glRasterPos3f(-450.0, 200, 0.0);
    writeBitmapString((void *)font, "Horizontal component of initial velocity: ");
    writeBitmapString((void *)font, theStringBuffer);

    floatToString(theStringBuffer, 4, v);
    glRasterPos3f(-450.0, 180.0, 0.0);
    writeBitmapString((void *)font, "Vertical component of initial velocity: ");
    writeBitmapString((void *)font, theStringBuffer);

    floatToString(theStringBuffer, 4, g);
    glRasterPos3f(-450.0, 160.0, 0.0);
    writeBitmapString((void *)font, "Gravitation: ");
    writeBitmapString((void *)font, theStringBuffer);
}

// Drawing routine.
void drawScene(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glPushMatrix();
    glTranslatef(0.0, 0.0, -580.0);
    writeData();
    glPopMatrix();

    glTranslatef(0.0, 0.0, -500.0);

    // Rotate scene.
    glRotatef(Zangle, 0.0, 0.0, 1.0);
    glRotatef(Yangle, 0.0, 1.0, 0.0);
    glRotatef(Xangle, 1.0, 0.0, 0.0);

    //Building the Target
    float R = 45.0; // Radius of Target.
    float ang = 0;  // Angle parameter.
    for (int j = 0; j < 5; j++)
    {
        ang = 0;
        switch (j)
        {
        case 0:
            glColor3f(1.0, 0.0, 0.0);
            break;
        case 1:
            glColor3f(1.0, 1.0, 0.0);
            break;
        case 2:
            glColor3f(0.0, 0.0, 1.0);
            break;
        case 3:
            glColor3f(0.0, 1.0, 0.0);
            break;
        case 4:
            glColor3f(1.0, 0.0, 0.0);
            break;
        }
        glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < numVertices; ++i)
        {
            glVertex3f(R * cos(ang), R * sin(ang), j * 1.0);
            ang += 2 * PI / numVertices;
        }
        glEnd();
        R -= 9;
    }
    glColor3f(1.0, 1.0, 1.0);
    //Preparing the arrow
    //Cylinder
    glPushMatrix();
    //glTranslatef(0.0, 0.0, arrowPos);
    //if(t == 0.0){
    //    glTranslatef(0.0, 0.0, 300.0);
    //}else{
    glTranslatef(X + 0.0, Y + (v * t - (g / 2.0) * t * t), Z - (h * t));
    //}
    glScalef(1.0, 1.0, 10.0);
    glutSolidCube(6.0);
    glPopMatrix();
    //Cone
    glPushMatrix();
    //glTranslatef(0.0, 0.0, arrowPos-30.0);
    //if(t == 0.0){
    //    glTranslatef( 0.0, 0.0, 270.0);
    //}else{
    glTranslatef(X + 0.0, Y + (v * t - (g / 2.0) * t * t), (Z - 30) - (h * t));
    //}
    glRotatef(180, 0.0, 1.0, 0.0);
    glutSolidCone(5.0, 10.0, 8.0, 10.0);
    glPopMatrix();

    glutSwapBuffers();
}

// Timer function.
void animate(int value)
{
    if (isAnimate)
    {
        /*
        arrowPos -= 10;
        if(arrowPos < 40)
            arrowPos += 300;
        */
        if ((v * t - (g / 2.0) * t * t) >= -45 && (h * t) <= 263)
        {
            t += 1.0;
            cout << (h * t) << endl;
        }
        glutPostRedisplay();
        glutTimerFunc(animationPeriod, animate, 1);
    }
}

// Mouse wheel callback routine.
void mouseWheel(int wheel, int direction, int x, int y)
{
    // Increment/decrement the global pointSize depending on the direction
    // of rotation of the mouse wheel.
    cout<<"Entered Mouse Wheel Function"<<endl;
    direction > 0 ? h++ : h--;
    if (h > 30.0)
        h = 30.0;
    if (h < 0)
        h = 0.0;
    glutPostRedisplay();
}

// Initialization routine.
void setup(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST); // Enable depth testing.
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, w / h, 5.0, 600.0);

    glMatrixMode(GL_MODELVIEW);

    // Pass the size of the OpenGL window to globals.
    width = w;
    height = h;
}

// Keyboard input processing routine.
void keyInput(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    case ' ':
        if (isAnimate)
            isAnimate = 0;
        else
        {
            isAnimate = 1;
            animate(1);
        }
        break;
    case 'x':
        Xangle += 5.0;
        if (Xangle > 360.0)
            Xangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'X':
        Xangle -= 5.0;
        if (Xangle < 0.0)
            Xangle += 360.0;
        glutPostRedisplay();
        break;
    case 'y':
        Yangle += 5.0;
        if (Yangle > 360.0)
            Yangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'Y':
        Yangle -= 5.0;
        if (Yangle < 0.0)
            Yangle += 360.0;
        glutPostRedisplay();
        break;
    case 'z':
        Zangle += 5.0;
        if (Zangle > 360.0)
            Zangle -= 360.0;
        glutPostRedisplay();
        break;
    case 'Z':
        Zangle -= 5.0;
        if (Zangle < 0.0)
            Zangle += 360.0;
        glutPostRedisplay();
        break;
    case 'r':
        isAnimate = 0;
        t = 0.0;
        glutPostRedisplay();
        break;
    default:
        break;
    }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
    /*if (key == GLUT_KEY_DOWN)
        animationPeriod += 5;
    if (key == GLUT_KEY_UP)
        if (animationPeriod > 5)
            animationPeriod -= 5;*/
    if (key == GLUT_KEY_UP)
        v += 0.05;
    ;
    if (key == GLUT_KEY_DOWN)
        if (v > 0.1)
            v -= 0.05;
    if (key == GLUT_KEY_RIGHT)
        h += 0.05;
    if (key == GLUT_KEY_LEFT)
        if (h > 0.1)
            h -= 0.05;
    if (key == GLUT_KEY_PAGE_UP)
        g += 0.05;
    if (key == GLUT_KEY_PAGE_DOWN)
        if (g > 0.1)
            g -= 0.05;
    glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
    cout << "Interaction:" << endl;
    cout << "Press space to toggle between animation on and off." << endl
         << "Press the x, X, y, Y, z, Z keys to rotate the scene." << endl
         << "Press right/left arrow kes to increase/decrease the initial horizontal velocity." << endl
         << "Press up/down arrow keys to increase/decrease the initial vertical velocity." << endl
         << "Press page up/down keys to increase/decrease gravitational acceleration." << endl
         << "Press r to reset." << endl;
}

// Main routine.
int main(int argc, char **argv)
{
    printInteraction();
    glutInit(&argc, argv);

    glutInitContextVersion(2, 1);
    glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Ekalaiva.cpp");
    glutDisplayFunc(drawScene);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyInput);
    glutSpecialFunc(specialKeyInput);

    // Register the mouse wheel callback function.
    glutMouseWheelFunc(mouseWheel);

    glewExperimental = GL_TRUE;
    glewInit();

    setup();

    glutMainLoop();
}
