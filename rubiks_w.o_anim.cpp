///////////////////////////////////////////////////////////////////////////
// Rubiks_w.o_anim.cpp
//
// This program, emulates the rubiks cube with the `
// use of the OpenGL pick matrix and selection mechanisms.
//
// Interaction:
// Press the x, X, y, Y, z, Z keys to rotate the scene.
// Click on the adjacent blocks to rotate the cubes
//
// Geeth Govind Srinivasulu.
///////////////////////////////////////////////////////////////////////////

#include <iostream>
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

using namespace std;

// Globals.
static float angle = 0.0;                           // Latitudinal angle.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; // Angles to rotate scene.
static int isAnimate = 0;                              // Animated?
static int animationPeriod = 100;                      // Time interval between frames.
static int highlightFrames = 10;                       // Number of frames to keep highlight.
static int isSelecting = 0;                            // In selection mode?
static int hits;                                       // Number of entries in hit buffer.
static unsigned int buffer[1024];                      // Hit buffer.
static unsigned int closestName = 0;                   // Name of closest hit.

//Parameters required to rotate the cube
int selections = 0;
int SelCube1 = 0, SelCube2 = 0;
//Block class
class Block
{
    private:
      int phase, xpos, ypos;
      float r, g, b;
      float Xrot, Yrot, Zrot;

    public:
      Block(int side, int Xpos, int Ypos, float R, float G, float B)
      {
            phase = side;
            xpos = Xpos;
            ypos = Ypos;
            r = R;
            g = G;
            b = B;
            Xrot = 0.0;
            Yrot = 0.0;
            Zrot = 0.0;
      }
      void placeBlocks(void);
      void SetRotation(float xRot, float yRot, float zRot);
      void PrintPos(void);
};
void Block::PrintPos()
{
      cout << "SIDE: " << phase << endl;
      cout << "XPOS: " << xpos << endl;
      cout << "YPOS: " << ypos << endl;
      cout << "R: " << r << endl;
      cout << "G: " << g << endl;
      cout << "B: " << b << endl;
      cout << "xRot: " << Xrot << endl;
      cout << "yRot: " << Yrot << endl;
      cout << "Zrot: " << Zrot << endl;
}
void Block::SetRotation(float xRot, float yRot, float zRot)
{
      Xrot += xRot;
      if (Xrot == 360)
            Xrot = 0.0;
      Yrot += yRot;
      if (Yrot == 360)
            Yrot = 0.0;
      Zrot += zRot;
      if (Zrot == 360)
            Zrot = 0.0;
}
void Block::placeBlocks(void)
{
      glPushMatrix();
      //glColor3f(r, g, b);
      glColor4f(0.0, 0.0, 0.0, 0.0);
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      glRotatef(Xrot, 1.0, 0.0, 0.0);
      glRotatef(Yrot, 0.0, 1.0, 0.0);
      glRotatef(Zrot, 0.0, 0.0, 1.0);
      switch (phase)
      {
      case 2:
            glRotatef(90.0, 0.0, 1.0, 0.0);
            break;
      case 3:
            glRotatef(180.0, 0.0, 1.0, 0.0);
            break;
      case 4:
            glRotatef(-90, 0.0, 1.0, 0.0);
            break;
      case 5:
            glRotatef(-90.0, 1.0, 0.0, 0.0);
            break;
      case 6:
            glRotatef(90.0, 1.0, 0.0, 0.0);
            break;
      }
      switch (xpos)
      {
      case 1:
            glTranslatef(0.0, 10.0, 15.5);
            break;
      case 2:
            glTranslatef(0.0, 0.0, 15.5);
            break;
      case 3:
            glTranslatef(0.0, -10.0, 15.5);
            break;
      }
      switch (ypos)
      {
      case 1:
            glTranslatef(-10.0, 0.0, 0.0);
            break;
      case 3:
            glTranslatef(10.0, 0.0, 0.0);
            break;
      }
      glBegin(GL_TRIANGLE_FAN);
      //glBegin(GL_LINE_LOOP);
      glVertex3f(-4.0, 4.0, 0.0);
      glVertex3f(4.0, 4.0, 0.0);
      glVertex3f(4.0, -4.0, 0.0);
      glVertex3f(-4.0, -4.0, 0.0);
      glEnd();
      glDisable(GL_BLEND);
      glPopMatrix();
}
class Angle
{
      public:
      int side;
      float angle;
      Angle(int s, float ang){
            side = s;
            angle = ang;
      }
};
// Cube class.
class Cube
{
    private:
      float a1, a2, a3, b1, b2, b3, c1, c2, c3, d1, d2, d3, e1, e2, e3, f1, f2, f3; // Colors for each side of the cube.
      int Xpos, Ypos, Zpos;                                                         //Position of the cube in the Rubiks cube.
      float size = 5.0;                                                             // Size of point.
      float xRot, yRot, zRot;
      // Vector of angles.
      vector<Angle> angles;
      // Iterator to traverse the angles array.
      vector<Angle>::iterator anglesIterator;

    public:
      Cube(float p1, float p2, float p3, float q1, float q2, float q3, float r1, float r2, float r3, float s1, float s2, float s3, float t1, float t2, float t3, float u1, float u2, float u3, int xpos, int ypos, int zpos)
      {
            a1 = p1;
            a2 = p2;
            a3 = p3;
            b1 = q1;
            b2 = q2;
            b3 = q3;
            c1 = r1;
            c2 = r2;
            c3 = r3;
            d1 = s1;
            d2 = s2;
            d3 = s3;
            e1 = t1;
            e2 = t2;
            e3 = t3;
            f1 = u1;
            f2 = u2;
            f3 = u3;
            Xpos = xpos;
            Ypos = ypos;
            Zpos = zpos;
            xRot = 0.0;
            yRot = 0.0;
            zRot = 0.0;
            angles.push_back(Angle(1, 0.0));
      }
      Cube(){};
      void setPos(int x, int y, int z)
      {
            Xpos = x;
            Ypos = y;
            Zpos = z;
      }
      void drawCube(); // Function to draw a point.
      void setRotation(float x, float y, float z);
      void PrintPos(void);
};
void Cube::PrintPos()
{
      cout<<Xpos<<","<<Ypos<<endl;
      anglesIterator = angles.begin();
      while(anglesIterator != angles.end()){
            cout<<"Side: "<<anglesIterator->side<<"Angle: "<<anglesIterator->angle<<endl;
            anglesIterator++;
      }
}
//Functioin to set the Amount of rotation to be done by the cube.
void Cube::setRotation(float x, float y, float z)
{
      if(x == 90.0 || x == -90.0){
            angles.push_back(Angle(1, x));
      }else if(y == 90.0 || y == -90.0){     
            angles.push_back(Angle(2, y));
      }else if (z == 90.0 || z == -90.0){
            angles.push_back(Angle(3, z));
      }
}
// Function to draw a cube.
void Cube::drawCube()
{
      glPushMatrix();
      anglesIterator = angles.end();
      anglesIterator--;
      while(anglesIterator != angles.begin()){
            switch(anglesIterator->side){
                  case 1:
                        glRotatef(anglesIterator->angle, 1.0, 0.0, 0.0);
                        break;
                  case 2:
                        glRotatef(anglesIterator->angle, 0.0, 1.0, 0.0);
                        break;
                  case 3:
                        glRotatef(anglesIterator->angle, 0.0, 0.0, 1.0);
                        break;
            }
            anglesIterator--;
      }
      switch (Zpos)
      {
      case 1:
            glTranslatef(-10.0, 0.0, 0.0);
            break;
      case 2:
            break;
      case 3:
            glTranslatef(10.0, 0.0, 0.0);
      }
      switch (Ypos)
      {
      case 1:
            glTranslatef(0.0, 10.0, 0.0);
            break;
      case 2:
            break;
      case 3:
            glTranslatef(0.0, -10.0, 0.0);
      }
      switch (Xpos)
      {
      case 1:
            glTranslatef(0.0, 0.0, 10.0);
            break;
      case 2:
            break;
      case 3:
            glTranslatef(0.0, 0.0, -10.0);
      }      
      //Side 1
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(a1, a2, a3);
      glVertex3f(-5.0, 5.0, 5.0);
      glVertex3f(5.0, 5.0, 5.0);
      glVertex3f(5.0, -5.0, 5.0);
      glVertex3f(-5.0, -5.0, 5.0);
      glEnd();
      //Side 2
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(b1, b2, b3);
      glVertex3f(5.0, 5.0, 5.0);
      glVertex3f(5.0, 5.0, -5.0);
      glVertex3f(5.0, -5.0, -5.0);
      glVertex3f(5.0, -5.0, 5.0);
      glEnd();
      //Side 3
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(c1, c2, c3);
      glVertex3f(5.0, 5.0, -5.0);
      glVertex3f(-5.0, 5.0, -5.0);
      glVertex3f(-5.0, -5.0, -5.0);
      glVertex3f(5.0, -5.0, -5.0);
      glEnd();
      //Side 4
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(d1, d2, d3);
      glVertex3f(-5.0, 5.0, -5.0);
      glVertex3f(-5.0, 5.0, 5.0);
      glVertex3f(-5.0, -5.0, 5.0);
      glVertex3f(-5.0, -5.0, -5.0);
      glEnd();
      //side 5
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(e1, e2, e3);
      glVertex3f(5.0, 5.0, -5.0);
      glVertex3f(5.0, 5.0, 5.0);
      glVertex3f(-5.0, 5.0, 5.0);
      glVertex3f(-5.0, 5.0, -5.0);
      glEnd();
      //side 6
      glBegin(GL_TRIANGLE_FAN);
      glColor3f(f1, f2, f3);
      glVertex3f(5.0, -5.0, 5.0);
      glVertex3f(5.0, -5.0, -5.0);
      glVertex3f(-5.0, -5.0, -5.0);
      glVertex3f(-5.0, -5.0, 5.0);
      glEnd();
      glColor3f(1.0, 1.0, 1.0);
      glLineWidth(size);
      glutWireCube(10);
      glPopMatrix();
}

Cube cubes[3][3][3] = {
    //Cube 1,1,1
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 1, 1, 1),
    //Cube 1,1,2
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 1, 1, 2),
    //Cube 1,1,3
    Cube(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 1, 1, 3),
    //Cube 1,2,1
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1, 2, 1),
    //Cube 1,2,2
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1, 2, 2),
    //Cube 1,2,3
    Cube(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1, 2, 3),
    //Cube 1,3,1
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 1, 3, 1),
    //Cube 1,3,2
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 1, 3, 2),
    //Cube 1,3,3
    Cube(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 1, 3, 3),

    //Cube 2,1,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 2, 1, 1),
    //Cube 2,1,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 2, 1, 2),
    //Cube 2,1,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 2, 1, 3),
    //Cube 2,2,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 2, 2, 1),
    //Cube 2,2,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 2, 2, 2),
    //Cube 2,2,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 2, 2, 3),
    //Cube 2,3,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 2, 3, 1),
    //Cube 2,3,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 2, 3, 2),
    //Cube 2,3,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 2, 3, 3),

    //Cube 3,1,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 3, 1, 1),
    //Cube 3,1,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 3, 1, 2),
    //Cube 3,1,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 3, 1, 3),
    //Cube 3,2,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 3, 2, 1),
    //Cube 3,2,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 3, 2, 2),
    //Cube 3,2,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 3, 2, 3),
    //Cube 3,3,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 3, 3, 1),
    //Cube 3,3,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 3, 3, 2),
    //Cube 3,3,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 3, 3, 3),
};
Cube copyCubes[3][3][3] = {
    //Cube 1,1,1
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 1, 1, 1),
    //Cube 1,1,2
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 1, 1, 2),
    //Cube 1,1,3
    Cube(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 1, 1, 3),
    //Cube 1,2,1
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1, 2, 1),
    //Cube 1,2,2
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1, 2, 2),
    //Cube 1,2,3
    Cube(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1, 2, 3),
    //Cube 1,3,1
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 1, 3, 1),
    //Cube 1,3,2
    Cube(1.0, 0.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 1, 3, 2),
    //Cube 1,3,3
    Cube(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 1, 3, 3),

    //Cube 2,1,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 2, 1, 1),
    //Cube 2,1,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 2, 1, 2),
    //Cube 2,1,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 2, 1, 3),
    //Cube 2,2,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 2, 2, 1),
    //Cube 2,2,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 2, 2, 2),
    //Cube 2,2,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 2, 2, 3),
    //Cube 2,3,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 2, 3, 1),
    //Cube 2,3,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 2, 3, 2),
    //Cube 2,3,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 2, 3, 3),

    //Cube 3,1,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 3, 1, 1),
    //Cube 3,1,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 3, 1, 2),
    //Cube 3,1,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.0, 1.0, 1.0, 0.3, 0.3, 0.3, 3, 1, 3),
    //Cube 3,2,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 3, 2, 1),
    //Cube 3,2,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 3, 2, 2),
    //Cube 3,2,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 3, 2, 3),
    //Cube 3,3,1
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 3, 3, 1),
    //Cube 3,3,2
    Cube(0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 3, 3, 2),
    //Cube 3,3,3
    Cube(0.3, 0.3, 0.3, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 1.0, 0.0, 1.0, 3, 3, 3),
};
Block blocks[6][9] = {
    {Block(1, 1, 1, 1.0, 0.0, 0.0),
     Block(1, 1, 2, 1.0, 0.0, 0.0),
     Block(1, 1, 3, 1.0, 0.0, 0.0),
     Block(1, 2, 1, 1.0, 0.0, 0.0),
     Block(1, 2, 2, 1.0, 0.0, 0.0),
     Block(1, 2, 3, 1.0, 0.0, 0.0),
     Block(1, 3, 1, 1.0, 0.0, 0.0),
     Block(1, 3, 2, 1.0, 0.0, 0.0),
     Block(1, 3, 3, 1.0, 0.0, 0.0)},
    {
        Block(2, 1, 1, 0.0, 1.0, 0.0),
        Block(2, 1, 2, 0.0, 1.0, 0.0),
        Block(2, 1, 3, 0.0, 1.0, 0.0),
        Block(2, 2, 1, 0.0, 1.0, 0.0),
        Block(2, 2, 2, 0.0, 1.0, 0.0),
        Block(2, 2, 3, 0.0, 1.0, 0.0),
        Block(2, 3, 1, 0.0, 1.0, 0.0),
        Block(2, 3, 2, 0.0, 1.0, 0.0),
        Block(2, 3, 3, 0.0, 1.0, 0.0),
    },
    {Block(3, 1, 1, 0.0, 0.0, 1.0),
     Block(3, 1, 2, 0.0, 0.0, 1.0),
     Block(3, 1, 3, 0.0, 0.0, 1.0),
     Block(3, 2, 1, 0.0, 0.0, 1.0),
     Block(3, 2, 2, 0.0, 0.0, 1.0),
     Block(3, 2, 3, 0.0, 0.0, 1.0),
     Block(3, 3, 1, 0.0, 0.0, 1.0),
     Block(3, 3, 2, 0.0, 0.0, 1.0),
     Block(3, 3, 3, 0.0, 0.0, 1.0)},
    {Block(4, 1, 1, 1.0, 1.0, 0.0),
     Block(4, 1, 2, 1.0, 1.0, 0.0),
     Block(4, 1, 3, 1.0, 1.0, 0.0),
     Block(4, 2, 1, 1.0, 1.0, 0.0),
     Block(4, 2, 2, 1.0, 1.0, 0.0),
     Block(4, 2, 3, 1.0, 1.0, 0.0),
     Block(4, 3, 1, 1.0, 1.0, 0.0),
     Block(4, 3, 2, 1.0, 1.0, 0.0),
     Block(4, 3, 3, 1.0, 1.0, 0.0)},
    {Block(5, 1, 1, 0.0, 1.0, 1.0),
     Block(5, 1, 2, 0.0, 1.0, 1.0),
     Block(5, 1, 3, 0.0, 1.0, 1.0),
     Block(5, 2, 1, 0.0, 1.0, 1.0),
     Block(5, 2, 2, 0.0, 1.0, 1.0),
     Block(5, 2, 3, 0.0, 1.0, 1.0),
     Block(5, 3, 1, 0.0, 1.0, 1.0),
     Block(5, 3, 2, 0.0, 1.0, 1.0),
     Block(5, 3, 3, 0.0, 1.0, 1.0)},
    {Block(6, 1, 1, 1.0, 0.0, 1.0),
     Block(6, 1, 2, 1.0, 0.0, 1.0),
     Block(6, 1, 3, 1.0, 0.0, 1.0),
     Block(6, 2, 1, 1.0, 0.0, 1.0),
     Block(6, 2, 2, 1.0, 0.0, 1.0),
     Block(6, 2, 3, 1.0, 0.0, 1.0),
     Block(6, 3, 1, 1.0, 0.0, 1.0),
     Block(6, 3, 2, 1.0, 0.0, 1.0),
     Block(6, 3, 3, 1.0, 0.0, 1.0)}};
void RingRotate(int x, int y, int z, int rot)
{
      //Row 1 Counter-Clockwise
      if (x == 1 && y == 0 && z == 0 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][x - 1][j].setRotation(0.0, -90.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j = 0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }            
            cubes[0][0][0] = copyCubes[0][0][2];
            cubes[0][0][1] = copyCubes[1][0][2];
            cubes[0][0][2] = copyCubes[2][0][2];
            cubes[1][0][0] = copyCubes[0][0][1];
            cubes[1][0][1] = copyCubes[1][0][1];
            cubes[1][0][2] = copyCubes[2][0][1];            
            cubes[2][0][0] = copyCubes[0][0][0];
            cubes[2][0][1] = copyCubes[1][0][0];
            cubes[2][0][2] = copyCubes[2][0][0];
      }
      //Row 1 Clock-wise
      else if (x == 1 && y == 0 && z == 0 && rot == 1)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][x - 1][j].setRotation(0.0, 90.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }      
            cubes[0][0][0] = copyCubes[2][0][0];
            cubes[0][0][1] = copyCubes[1][0][0];
            cubes[0][0][2] = copyCubes[0][0][0];
            cubes[1][0][0] = copyCubes[2][0][1];
            cubes[1][0][1] = copyCubes[1][0][1];
            cubes[1][0][2] = copyCubes[0][0][1];            
            cubes[2][0][0] = copyCubes[2][0][2];
            cubes[2][0][1] = copyCubes[1][0][2];
            cubes[2][0][2] = copyCubes[0][0][2];
      }
      //Row 2 Counter-Clockwise
      else if (x == 2 && y == 0 && z == 0 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][x - 1][j].setRotation(0.0, -90.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }
            cubes[0][1][0] = copyCubes[0][1][2];
            cubes[0][1][1] = copyCubes[1][1][2];
            cubes[0][1][2] = copyCubes[2][1][2];
            cubes[1][1][0] = copyCubes[0][1][1];
            cubes[1][1][1] = copyCubes[1][1][1];
            cubes[1][1][2] = copyCubes[2][1][1];            
            cubes[2][1][0] = copyCubes[0][1][0];
            cubes[2][1][1] = copyCubes[1][1][0];
            cubes[2][1][2] = copyCubes[2][1][0];
      }
      //Row 2 Clock-wise Rotation
      else if (x == 2 && y == 0 && z == 0 && rot == 1)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][x - 1][j].setRotation(0.0, 90.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }
            cubes[0][1][0] = copyCubes[2][1][0];
            cubes[0][1][1] = copyCubes[1][1][0];
            cubes[0][1][2] = copyCubes[0][1][0];
            cubes[1][1][0] = copyCubes[2][1][1];
            cubes[1][1][1] = copyCubes[1][1][1];
            cubes[1][1][2] = copyCubes[0][1][1];            
            cubes[2][1][0] = copyCubes[2][1][2];
            cubes[2][1][1] = copyCubes[1][1][2];
            cubes[2][1][2] = copyCubes[0][1][2];
      }
      //Row 3 Counter-Clockwise Rotation
      else if (x == 3 && y == 0 && z == 0 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][x - 1][j].setRotation(0.0, -90.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }
            cubes[0][2][0] = copyCubes[0][2][2];
            cubes[0][2][1] = copyCubes[1][2][2];
            cubes[0][2][2] = copyCubes[2][2][2];
            cubes[1][2][0] = copyCubes[0][2][1];
            cubes[1][2][1] = copyCubes[1][2][1];
            cubes[1][2][2] = copyCubes[2][2][1];            
            cubes[2][2][0] = copyCubes[0][2][0];
            cubes[2][2][1] = copyCubes[1][2][0];
            cubes[2][2][2] = copyCubes[2][2][0];
      }
      //Row 3 Clock-Wise Rotation.
      else if (x == 3 && y == 0 && z == 0 && rot == 1)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][x - 1][j].setRotation(0.0, 90.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }
            cubes[0][2][0] = copyCubes[2][2][0];
            cubes[0][2][1] = copyCubes[1][2][0];
            cubes[0][2][2] = copyCubes[0][2][0];
            cubes[1][2][0] = copyCubes[2][2][1];
            cubes[1][2][1] = copyCubes[1][2][1];
            cubes[1][2][2] = copyCubes[0][2][1];            
            cubes[2][2][0] = copyCubes[2][2][2];
            cubes[2][2][1] = copyCubes[1][2][2];
            cubes[2][2][2] = copyCubes[0][2][2];
      }
      //Column 1 Counter-Clockwise.
      else if (x == 0 && y == 1 && z == 0 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][j][y - 1].setRotation(-90.0, 0.0, 0.0);
                  }
            } 
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }       
            cubes[0][0][0] = copyCubes[0][2][0];
            cubes[0][1][0] = copyCubes[1][2][0];
            cubes[0][2][0] = copyCubes[2][2][0];
            cubes[1][0][0] = copyCubes[0][1][0];
            cubes[1][1][0] = copyCubes[1][1][0];
            cubes[1][2][0] = copyCubes[2][1][0];            
            cubes[2][0][0] = copyCubes[0][0][0];
            cubes[2][1][0] = copyCubes[1][0][0];
            cubes[2][2][0] = copyCubes[2][0][0];    

      }
      //Column 1 Clock-Wise.
      else if (x == 0 && y == 1 && z == 0 && rot == 1)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][j][y - 1].setRotation(90.0, 0.0, 0.0);
                  }
            }    
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }        
            cubes[0][0][0] = copyCubes[2][0][0];
            cubes[0][1][0] = copyCubes[1][0][0];
            cubes[0][2][0] = copyCubes[0][0][0];
            cubes[1][0][0] = copyCubes[2][1][0];
            cubes[1][1][0] = copyCubes[1][1][0];
            cubes[1][2][0] = copyCubes[0][1][0];            
            cubes[2][0][0] = copyCubes[2][2][0];
            cubes[2][1][0] = copyCubes[1][2][0];
            cubes[2][2][0] = copyCubes[0][2][0];      
      }
      //Column 2 Counter- ClockWise
      else if (x == 0 && y == 2 && z == 0 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][j][y - 1].setRotation(-90.0, 0.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }      
            cubes[0][0][1] = copyCubes[0][2][1];
            cubes[0][1][1] = copyCubes[1][2][1];
            cubes[0][2][1] = copyCubes[2][2][1];
            cubes[1][0][1] = copyCubes[0][1][1];
            cubes[1][1][1] = copyCubes[1][1][1];
            cubes[1][2][1] = copyCubes[2][1][1];            
            cubes[2][0][1] = copyCubes[0][0][1];
            cubes[2][1][1] = copyCubes[1][0][1];
            cubes[2][2][1] = copyCubes[2][0][1];  
      }
      //column 2 Clock-Wise
      else if (x == 0 && y == 2 && z == 0 && rot == 1)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][j][y - 1].setRotation(90.0, 0.0, 0.0);
                  }
            }   
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }           
            cubes[0][0][1] = copyCubes[2][0][1];
            cubes[0][1][1] = copyCubes[1][0][1];
            cubes[0][2][1] = copyCubes[0][0][1];
            cubes[1][0][1] = copyCubes[2][1][1];
            cubes[1][1][1] = copyCubes[1][1][1];
            cubes[1][2][1] = copyCubes[0][1][1];            
            cubes[2][0][1] = copyCubes[2][2][1];
            cubes[2][1][1] = copyCubes[1][2][1];
            cubes[2][2][1] = copyCubes[0][2][1];        
      }
      //Column 3 Counter-ClockWise
      else if (x == 0 && y == 3 && z == 0 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][j][y - 1].setRotation(-90.0, 0.0, 0.0);
                  }
            }  
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }         
            cubes[0][0][2] = copyCubes[0][2][2];
            cubes[0][1][2] = copyCubes[1][2][2];
            cubes[0][2][2] = copyCubes[2][2][2];
            cubes[1][0][2] = copyCubes[0][1][2];
            cubes[1][1][2] = copyCubes[1][1][2];
            cubes[1][2][2] = copyCubes[2][1][2];            
            cubes[2][0][2] = copyCubes[0][0][2];
            cubes[2][1][2] = copyCubes[1][0][2];
            cubes[2][2][2] = copyCubes[2][0][2]; 
      }
      //Column 3 ClockWise.
      else if (x == 0 && y == 3 && z == 0 && rot == 1)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[i][j][y - 1].setRotation(90.0, 0.0, 0.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }         
            cubes[0][0][2] = copyCubes[2][0][2];
            cubes[0][1][2] = copyCubes[1][0][2];
            cubes[0][2][2] = copyCubes[0][0][2];
            cubes[1][0][2] = copyCubes[2][1][2];
            cubes[1][1][2] = copyCubes[1][1][2];
            cubes[1][2][2] = copyCubes[0][1][2];            
            cubes[2][0][2] = copyCubes[2][2][2];
            cubes[2][1][2] = copyCubes[1][2][2];
            cubes[2][2][2] = copyCubes[0][2][2];
      }
      //Z Column 1 Counter-Clockwise.
      else if (x == 0 && y == 0 && z == 1 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[z-1][i][j].setRotation(0.0, 0.0, 90.0);
                  }
            }     
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }        
            cubes[0][0][0] = copyCubes[0][0][2];
            cubes[0][0][1] = copyCubes[0][1][2];
            cubes[0][0][2] = copyCubes[0][2][2];
            cubes[0][1][0] = copyCubes[0][0][1];
            cubes[0][1][1] = copyCubes[0][1][1];
            cubes[0][1][2] = copyCubes[0][2][1];            
            cubes[0][2][0] = copyCubes[0][0][0];
            cubes[0][2][1] = copyCubes[0][1][0];
            cubes[0][2][2] = copyCubes[0][2][0];

      }
      //Z Column 1 Clock-Wise.
      else if (x == 0 && y == 0 && z == 1 && rot == 1)
      {            
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[z-1][i][j].setRotation(0.0, 0.0, -90.0);
                  }
            }          
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }         
            cubes[0][0][0] = copyCubes[0][2][0];
            cubes[0][0][1] = copyCubes[0][1][0];
            cubes[0][0][2] = copyCubes[0][0][0];
            cubes[0][1][0] = copyCubes[0][2][1];
            cubes[0][1][1] = copyCubes[0][1][1];
            cubes[0][1][2] = copyCubes[0][0][1];            
            cubes[0][2][0] = copyCubes[0][2][2];
            cubes[0][2][1] = copyCubes[0][1][2];
            cubes[0][2][2] = copyCubes[0][0][2];
      }
      //Z Column 2 Counter- ClockWise
      else if (x == 0 && y == 0 && z == 2 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[z-1][i][j].setRotation(0.0, 0.0, 90.0);
                  }
            }     
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }       
            cubes[1][0][0] = copyCubes[1][0][2];
            cubes[1][0][1] = copyCubes[1][1][2];
            cubes[1][0][2] = copyCubes[1][2][2];
            cubes[1][1][0] = copyCubes[1][0][1];
            cubes[1][1][1] = copyCubes[1][1][1];
            cubes[1][1][2] = copyCubes[1][2][1];            
            cubes[1][2][0] = copyCubes[1][0][0];
            cubes[1][2][1] = copyCubes[1][1][0];
            cubes[1][2][2] = copyCubes[1][2][0];   
      }
      //Z column 2 Clock-Wise
      else if (x == 0 && y == 0 && z == 2 && rot == 1)
      {           
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[z-1][i][j].setRotation(0.0, 0.0, -90.0);
                  }
            }         
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }       
            cubes[1][0][0] = copyCubes[1][2][0];
            cubes[1][0][1] = copyCubes[1][1][0];
            cubes[1][0][2] = copyCubes[1][0][0];
            cubes[1][1][0] = copyCubes[1][2][1];
            cubes[1][1][1] = copyCubes[1][1][1];
            cubes[1][1][2] = copyCubes[1][0][1];            
            cubes[1][2][0] = copyCubes[1][2][2];
            cubes[1][2][1] = copyCubes[1][1][2];
            cubes[1][2][2] = copyCubes[1][0][2];
      }
      //Z Column 3 Counter-ClockWise
      else if (x == 0 && y == 0 && z == 3 && rot == 0)
      {
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[z-1][i][j].setRotation(0.0, 0.0, 90.0);
                  }
            }
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }     
            cubes[2][0][0] = copyCubes[2][0][2];
            cubes[2][0][1] = copyCubes[2][1][2];
            cubes[2][0][2] = copyCubes[2][2][2];
            cubes[2][1][0] = copyCubes[2][0][1];
            cubes[2][1][1] = copyCubes[2][1][1];
            cubes[2][1][2] = copyCubes[2][2][1];            
            cubes[2][2][0] = copyCubes[2][0][0];
            cubes[2][2][1] = copyCubes[2][1][0];
            cubes[2][2][2] = copyCubes[2][2][0];
      }
      //Z Column 3 ClockWise.
      else if (x == 0 && y == 0 && z == 3 && rot == 1)
      {           
            for (int i = 0; i < 3; i++)
            {
                  for (int j = 0; j < 3; j++)
                  {
                        cubes[z-1][i][j].setRotation(0.0, 0.0, -90.0);
                  }
            }                
            for(int i = 0;i<3;i++){
                  for(int j =  0;j<3;j++){
                        for(int k = 0;k<3;k++){
                              copyCubes[i][j][k] = cubes[i][j][k];
                        }
                  }
            }         
            cubes[2][0][0] = copyCubes[2][2][0];
            cubes[2][0][1] = copyCubes[2][1][0];
            cubes[2][0][2] = copyCubes[2][0][0];
            cubes[2][1][0] = copyCubes[2][2][1];
            cubes[2][1][1] = copyCubes[2][1][1];
            cubes[2][1][2] = copyCubes[2][0][1];            
            cubes[2][2][0] = copyCubes[2][2][2];
            cubes[2][2][1] = copyCubes[2][1][2];
            cubes[2][2][2] = copyCubes[2][0][2];
      }
      glutPostRedisplay();
}
//Calculating the rotations to be done and setting the values to the cubes.
void PerformRotation(void)
{
      switch (SelCube1)
      {
      case 1:
            switch (SelCube2)
            {

            case 30:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 43:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 2:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 4:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 2:
            switch (SelCube2)
            {
            case 1:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 44:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 3:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 5:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 3:
            switch (SelCube2)
            {
            case 2:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 45:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 10:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 6:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 4:
            switch (SelCube2)
            {
            case 33:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 1:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 5:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 7:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 5:
            switch (SelCube2)
            {
            case 4:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 2:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 6:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 8:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 6:
            switch (SelCube2)
            {
            case 5:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 3:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 13:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 9:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 7:
            switch (SelCube2)
            {
            case 36:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 4:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 8:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 46:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 8:
            switch (SelCube2)
            {
            case 7:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 5:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 9:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 47:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 9:
            switch (SelCube2)
            {
            case 8:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 6:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 16:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 48:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 10:
            switch (SelCube2)
            {
            case 3:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 45:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 11:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 13:
                  RingRotate(0, 0, 1, 1);
                  break;
            }
            break;
      case 11:
            switch (SelCube2)
            {
            case 10:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 42:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 12:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 14:
                  RingRotate(0, 0, 2, 1);
                  break;
            }
            break;
      case 12:
            switch (SelCube2)
            {
            case 11:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 39:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 19:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 15:
                  RingRotate(0, 0, 3, 1);
                  break;
            }
            break;
      case 13:
            switch (SelCube2)
            {
            case 6:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 10:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 14:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 16:
                  RingRotate(0, 0, 1, 1);
                  break;
            }
            break;
      case 14:
            switch (SelCube2)
            {
            case 13:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 11:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 15:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 17:
                  RingRotate(0, 0, 2, 1);
                  break;
            }
            break;
      case 15:
            switch (SelCube2)
            {
            case 14:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 12:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 22:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 18:
                  RingRotate(0, 0, 3, 1);
                  break;
            }
            break;
      case 16:
            switch (SelCube2)
            {
            case 9:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 13:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 17:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 48:
                  RingRotate(0, 0, 1, 1);
                  break;
            }
            break;
      case 17:
            switch (SelCube2)
            {
            case 16:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 14:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 18:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 51:
                  RingRotate(0, 0, 2, 1);
                  break;
            }
            break;
      case 18:
            switch (SelCube2)
            {
            case 17:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 15:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 25:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 54:
                  RingRotate(0, 0, 3, 1);
                  break;
            }
            break;
      case 19:
            switch (SelCube2)
            {
            case 12:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 39:
                  RingRotate(0, 3, 0, 1);
                  break;
            case 20:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 22:
                  RingRotate(0, 3, 0, 0);
                  break;
            }
            break;
      case 20:
            switch (SelCube2)
            {
            case 19:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 38:
                  RingRotate(0, 2, 0, 1);
                  break;
            case 21:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 23:
                  RingRotate(0, 2, 0, 0);
                  break;
            }
            break;
      case 21:
            switch (SelCube2)
            {
            case 20:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 37:
                  RingRotate(0, 1, 0, 1);
                  break;
            case 28:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 24:
                  RingRotate(0, 1, 0, 0);
                  break;
            }
            break;
      case 22:
            switch (SelCube2)
            {
            case 15:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 19:
                  RingRotate(0, 3, 0, 1);
                  break;
            case 23:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 25:
                  RingRotate(0, 3, 0, 0);
                  break;
            }
            break;
      case 23:
            switch (SelCube2)
            {
            case 22:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 20:
                  RingRotate(0, 2, 0, 1);
                  break;
            case 24:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 26:
                  RingRotate(0, 2, 0, 0);
                  break;
            }
            break;
      case 24:
            switch (SelCube2)
            {
            case 23:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 21:
                  RingRotate(0, 1, 0, 1);
                  break;
            case 31:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 27:
                  RingRotate(0, 1, 0, 0);
                  break;
            }
            break;
      case 25:
            switch (SelCube2)
            {
            case 18:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 22:
                  RingRotate(0, 3, 0, 1);
                  break;
            case 26:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 54:
                  RingRotate(0, 3, 0, 0);
                  break;
            }
            break;
      case 26:
            switch (SelCube2)
            {
            case 25:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 23:
                  RingRotate(0, 2, 0, 1);
                  break;
            case 27:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 53:
                  RingRotate(0, 2, 0, 0);
                  break;
            }
            break;
      case 27:
            switch (SelCube2)
            {
            case 26:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 24:
                  RingRotate(0, 1, 0, 1);
                  break;
            case 34:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 52:
                  RingRotate(0, 1, 0, 0);
                  break;
            }
            break;
      case 28:
            switch (SelCube2)
            {
            case 21:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 37:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 29:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 31:
                  RingRotate(0, 0, 3, 0);
                  break;
            }
            break;
      case 29:
            switch (SelCube2)
            {
            case 28:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 40:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 30:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 32:
                  RingRotate(0, 0, 2, 0);
                  break;
            }
            break;
      case 30:
            switch (SelCube2)
            {
            case 29:
                  RingRotate(1, 0, 0, 0);
                  break;
            case 43:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 1:
                  RingRotate(1, 0, 0, 1);
                  break;
            case 33:
                  RingRotate(0, 0, 1, 0);
                  break;
            }
            break;
      case 31:
            switch (SelCube2)
            {
            case 24:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 28:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 32:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 34:
                  RingRotate(0, 0, 3, 0);
                  break;
            }
            break;
      case 32:
            switch (SelCube2)
            {
            case 31:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 29:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 33:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 35:
                  RingRotate(0, 0, 2, 0);
                  break;
            }
            break;
      case 33:
            switch (SelCube2)
            {
            case 32:
                  RingRotate(2, 0, 0, 0);
                  break;
            case 30:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 4:
                  RingRotate(2, 0, 0, 1);
                  break;
            case 36:
                  RingRotate(0, 0, 1, 0);
                  break;
            }
            break;
      case 34:
            switch (SelCube2)
            {
            case 27:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 31:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 35:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 52:
                  RingRotate(0, 0, 3, 0);
                  break;
            }
            break;
      case 35:
            switch (SelCube2)
            {
            case 34:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 32:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 36:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 49:
                  RingRotate(0, 0, 2, 0);
                  break;
            }
            break;
      case 36:
            switch (SelCube2)
            {
            case 35:
                  RingRotate(3, 0, 0, 0);
                  break;
            case 33:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 7:
                  RingRotate(3, 0, 0, 1);
                  break;
            case 46:
                  RingRotate(0, 0, 1, 0);
                  break;
            }
            break;
      case 37:
            switch (SelCube2)
            {
            case 28:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 21:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 38:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 40:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 38:
            switch (SelCube2)
            {
            case 37:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 20:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 39:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 41:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 39:
            switch (SelCube2)
            {
            case 38:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 19:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 12:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 42:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 40:
            switch (SelCube2)
            {
            case 29:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 37:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 41:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 43:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 41:
            switch (SelCube2)
            {
            case 40:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 38:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 42:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 44:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 42:
            switch (SelCube2)
            {
            case 41:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 39:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 11:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 45:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 43:
            switch (SelCube2)
            {
            case 30:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 40:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 44:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 1:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 44:
            switch (SelCube2)
            {
            case 43:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 41:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 45:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 2:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 45:
            switch (SelCube2)
            {
            case 44:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 42:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 10:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 3:
                  RingRotate(0, 3, 0, 1);
                  break;
                  break;
            }
            break;
      case 46:
            switch (SelCube2)
            {
            case 36:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 7:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 47:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 49:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 47:
            switch (SelCube2)
            {
            case 46:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 8:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 48:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 50:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 48:
            switch (SelCube2)
            {
            case 47:
                  RingRotate(0, 0, 1, 1);
                  break;
            case 9:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 16:
                  RingRotate(0, 0, 1, 0);
                  break;
            case 51:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 49:
            switch (SelCube2)
            {
            case 35:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 46:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 50:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 52:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 50:
            switch (SelCube2)
            {
            case 49:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 47:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 51:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 53:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 51:
            switch (SelCube2)
            {
            case 50:
                  RingRotate(0, 0, 2, 1);
                  break;
            case 48:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 17:
                  RingRotate(0, 0, 2, 0);
                  break;
            case 54:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      case 52:
            switch (SelCube2)
            {
            case 34:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 49:
                  RingRotate(0, 1, 0, 0);
                  break;
            case 53:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 27:
                  RingRotate(0, 1, 0, 1);
                  break;
            }
            break;
      case 53:
            switch (SelCube2)
            {
            case 52:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 50:
                  RingRotate(0, 2, 0, 0);
                  break;
            case 54:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 26:
                  RingRotate(0, 2, 0, 1);
                  break;
            }
            break;
      case 54:
            switch (SelCube2)
            {
            case 53:
                  RingRotate(0, 0, 3, 1);
                  break;
            case 51:
                  RingRotate(0, 3, 0, 0);
                  break;
            case 18:
                  RingRotate(0, 0, 3, 0);
                  break;
            case 25:
                  RingRotate(0, 3, 0, 1);
                  break;
            }
            break;
      }
}
// Drawing routine.
void drawBallAndTorus(void)
{
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glLoadIdentity();
      glTranslatef(0.0, 0.0, -50.0);

      // Rotate scene.
      glRotatef(Zangle, 0.0, 0.0, 1.0);
      glRotatef(Yangle, 0.0, 1.0, 0.0);
      glRotatef(Xangle, 1.0, 0.0, 0.0);

      /*cubes[2][0][0].drawCube();
      cubes[0][0][0].drawCube();*/
      for (int i = 0; i < 3; i++)
      {
            for (int j = 0; j < 3; j++)
            {
                  for (int k = 0; k < 3; k++)
                  {
                        cubes[i][j][k].drawCube();
                  }
            }
      }
      int naam = 1;
      for (int i = 0; i < 6; i++)
      {
            for (int j = 0; j < 9; j++)
            {
                  int name = naam;
                  if (isSelecting)
                        glLoadName(name);
                  if ((highlightFrames > 0) && (closestName == name))
                  {
                        //setting the cubes and the direction in which the cubes have to be rotated.
                        switch (selections)
                        {
                        case 0:
                              SelCube1 = name;
                              selections++;
                              break;
                        case 1:
                              if (SelCube1 != name)
                              {
                                    SelCube2 = name;
                                    selections++;
                                    PerformRotation();
                              }
                              break;
                        case 2:
                              if (name != SelCube2)
                              {
                                    SelCube1 = name;
                                    selections = 1;
                              }
                              break;
                        }
                        glColor3f(1.0, 1.0, 1.0); // Highlight if selected.
                  }
                  blocks[i][j].placeBlocks();
                  naam++;
            }
      }
      if (isSelecting)
            glPopName(); // Clear name stack.
      /*else
            cubes[0][0][0].PrintPos();*/
      glutSwapBuffers();
}

// Drawing routine.
void drawScene(void)
{
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glColor3f(1.0, 1.0, 1.0);

      // Draw ball and torus in rendering mode.
      isSelecting = 0;
      drawBallAndTorus();
}

// Process hit buffer to find record with smallest min-z value.
void findClosestHit(int hits, unsigned int buffer[])
{
      unsigned int *ptr, minZ;

      minZ = 0xffffffff; // 2^32 - 1
      ptr = buffer;
      closestName = 0;
      for (int i = 0; i < hits; i++)
      {
            ptr++;
            if (*ptr < minZ)
            {
                  minZ = *ptr;
                  ptr += 2;
                  closestName = *ptr;
                  ptr++;
            }
            else
                  ptr += 3;
      }
      if (closestName != 0)
            highlightFrames = 10;
}

// The mouse callback routine.
void pickFunction(int button, int state, int x, int y)
{
      int viewport[4]; // Viewport data.

      if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN)
            return; // Don't react unless left button is pressed.

      glGetIntegerv(GL_VIEWPORT, viewport); // Get viewport data.

      glSelectBuffer(1024, buffer);  // Specify buffer to write hit records in selection mode
      (void)glRenderMode(GL_SELECT); // Enter selection mode.

      // Save the viewing volume defined in the resize routine.
      glMatrixMode(GL_PROJECTION);
      glPushMatrix();

      // Define a viewing volume corresponding to selecting in 3 x 3 region around the cursor.
      glLoadIdentity();
      gluPickMatrix((float)x, (float)(viewport[3] - y), 3.0, 3.0, viewport);
      glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0); // Copied from the reshape routine.

      glMatrixMode(GL_MODELVIEW); // Return to modelview mode before drawing.
      glLoadIdentity();

      glInitNames(); // Initializes the name stack to empty.
      glPushName(0); // Puts name 0 on top of stack.

      // Determine hits by calling drawBallAndTorus() so that names are assigned.
      isSelecting = 1;
      drawBallAndTorus();

      hits = glRenderMode(GL_RENDER); // Return to rendering mode, returning number of hits.

      // Restore viewing volume of the resize routine and return to modelview mode.
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);

      // Determine closest of the hit objects (if any).
      findClosestHit(hits, buffer);

      glutPostRedisplay();
}

// Timer function.
void animate(int value)
{
      if (isAnimate)
      {
      }
      if (highlightFrames > 0)
            highlightFrames--;

      glutPostRedisplay();
      glutTimerFunc(animationPeriod, animate, 1);
}

// Initialization routine.
void setup(void)
{
      glClearColor(0.0, 0.0, 0.0, 0.0);
      //glClearColor(1.0, 1.0, 1.0, 0.0);
      glEnable(GL_DEPTH_TEST); // Enable depth testing.

      glutTimerFunc(5, animate, 1);
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
      glViewport(0, 0, w, h);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      glFrustum(-5.0, 5.0, -5.0, 5.0, 5.0, 100.0);

      glMatrixMode(GL_MODELVIEW);
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
                  isAnimate = 1;
            glutPostRedisplay();
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
      default:
            break;
      }
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
      if (key == GLUT_KEY_DOWN)
            animationPeriod += 5;
      if (key == GLUT_KEY_UP)
            if (animationPeriod > 5)
                  animationPeriod -= 5;
      glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
      cout << "Interaction:" << endl;
      cout << "Press space to toggle between animation on and off." << endl
           << "Press the up/down arrow keys to speed up/slow down animation." << endl
           << "Press the x, X, y, Y, z, Z keys to rotate the scene." << endl
           << endl;
}

// Main routine.
int main(int argc, char **argv)
{
      printInteraction();
      glutInit(&argc, argv);

      glutInitContextVersion(2, 1);
      glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

      glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
      glutInitWindowSize(500, 500);
      glutInitWindowPosition(100, 100);
      glutCreateWindow("Rubiks Cube");
      glutDisplayFunc(drawScene);
      glutReshapeFunc(resize);
      glutKeyboardFunc(keyInput);
      glutSpecialFunc(specialKeyInput);
      glutMouseFunc(pickFunction);

      glewExperimental = GL_TRUE;
      glewInit();

      setup();

      glutMainLoop();
}
