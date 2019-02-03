////////////////////////////////////////////////////////////////
// Towers of Hanoi.cpp
//
// This program draws a ball that flies around a torus.
//
// Interaction:
// Press space to toggle between animation on and off.
// Press the up/down arrow keys to speed up/slow down animation.
// Press the x, X, y, Y, z, Z keys to rotate the scene.
//
// Geeth Govind Srinivasulu.
////////////////////////////////////////////////////////////////

#include <iostream>
#include <vector>
#include <list>
#include <unistd.h>

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
float angle = 0.0;
static float longAngle = 0.0;						   	// Longitudinal angle.
static float Xangle = 0.0, Yangle = 0.0, Zangle = 0.0; 	// Angles to rotate scene.
static int isAnimate = 0;							  	// Animated?
static int animationPeriod = 100; 						// Time interval between frames.
int rings = 5;											//Total number of rings
int flag = 0;											//Flag used for certain operations
float xTrans = 0.0, yTrans = 0.0, zTrans = 0.0;			//For animation
int toMove = 0;											//Which idisk is currently moving
int fromMoveFlag = 0, toMoveFlag = 0, toPlaceFlag = 0, changeFlag = 0;	//Flags for animation
//Class for storing the steps to be performed.
class Step
{
  private:
	int ring;
	char from, to;

  public:
	Step(int r, char f, char t)
	{
		ring = r;
		from = f;
		to = t;
	}
	int getRingNo(void);
	char getFrom(void);
	char getTo(void);
};
//Class for the properties of the desk.
class Ring
{
	private:
	float radius, Radius;
	int num;

  public:
	Ring(float r, float R, int n)
	{
		radius = r;
		Radius = R;
		num = n;
	}
	void drawRing(int no);
	int getNum(void);
};
//Class for operations and properties of the Bars.
class Bar
{
  private:
	// Vector of Rings.
	list<Ring> barRings;
	// Iterator to traverse the barRings array.
	list<Ring>::iterator ringsIterator;
	char name;

  public:
	Bar(char naam)
	{
		name = naam;
	}
	void displayRings(void);
	void addRing(Ring r);
	Ring removeRing(void);
	char displayName();
	void listRings(void);
	Ring getLastRing(void);
	int getTotalRings();
};
// Vector of Disks.
vector<Ring> disks;
// Iterator to traverse the disks array.
vector<Ring>::iterator disksIterator;
// Vector of Rings.
list<Step> steps;
Bar frmBar = Bar('a');
Bar tBar = Bar('b');
Bar aBar = Bar('c');
int Step::getRingNo()
{
	return ring;
}
char Step::getFrom()
{
	return from;
}
char Step::getTo()
{
	return to;
}
int Ring::getNum()
{
	return num;
}
void Ring::drawRing(int no)
{
	srand(num+5050);
	glColor3f((float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX, (float)rand() / (float)RAND_MAX);
	if (isAnimate && no == num){
		glTranslatef(xTrans, yTrans, zTrans);
	}
	glutSolidTorus(radius, Radius, 20.0, 20.0);
}
int Bar::getTotalRings()
{
	return barRings.size();
}
Ring Bar::getLastRing(void)
{
	return (disks[barRings.back().getNum()]);
}
void Bar::listRings()
{
	list<Ring>::iterator it;
	for (it = barRings.begin(); it != barRings.end(); ++it)
		cout << '\t' << it->getNum();
	cout << '\n';
}
char Bar::displayName()
{
	return name;
}
void Bar::displayRings(void)
{
	glColor3f(1.0, 1.0, 1.0);
	glutWireCylinder(2.0, (rings+1) * 3.0, 20.0, 20.0);
	ringsIterator = barRings.begin();
	int i = 0;
	while (ringsIterator != barRings.end())
	{
		glPushMatrix();
		glTranslatef(0.0, 0.0, i * (3.0));
		ringsIterator->drawRing(toMove);
		glPopMatrix();
		ringsIterator++;
		i++;
	}
}
void Bar::addRing(Ring r)
{
	barRings.push_back(r);
}
Ring Bar::removeRing()
{
	Ring poped = disks[barRings.back().getNum()];
	barRings.pop_back();
	return poped;
}
//Method which recursively finds out the steps to be performed by the program.
void towers(int ring, char frompeg, char topeg, char auxpeg)
{
	if (ring == 1)
	{
		//For printing the steps to be performed.
		//printf("Move disk 1 from peg %c to peg %c\n", frompeg, topeg);
		steps.push_back(Step(1, frompeg, topeg));
		return;
	}
	towers(ring - 1, frompeg, auxpeg, topeg);
	//For printing the steps to be performed.
	//printf("Move disk %d from peg %c to peg %c\n", ring, frompeg, topeg);
	steps.push_back(Step(ring, frompeg, topeg));
	towers(ring - 1, auxpeg, topeg, frompeg);
}
// Drawing routine.
void drawScene(void)
{
	flag = 0;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -200.0);

	// Rotate scene.
	glRotatef(Zangle, 0.0, 0.0, 1.0);
	glRotatef(Yangle, 0.0, 1.0, 0.0);
	glRotatef(Xangle, 1.0, 0.0, 0.0);

	//To rotate the objects so that they look in the proper way when the scene is Played.
	glRotatef(-90.0, 1.0, 0.0, 0.0);
	glPushMatrix();
	glTranslatef(-(rings * 10.0), 0.0, 0.0);
	frmBar.displayRings(); //First Bar (From Bar)
	glPopMatrix();
	glPushMatrix();
	tBar.displayRings();	//Second Bar (To Bar)
	glPopMatrix();
	glPushMatrix();
	glTranslatef(rings * 10.0, 0.0, 0.0);
	aBar.displayRings();	//Third Bar (Auxilary Bar)
	glPopMatrix();
	glutSwapBuffers();
	flag = 1;
}

// Timer function.
void animate(int value)
{
	if (isAnimate)
	{
		char f = steps.back().getFrom();
		char t = steps.back().getTo();
		//Setting which bar is going to be animated.
		switch(f){
			case 'a':
				toMove = frmBar.getLastRing().getNum();
				break;
			case 'b':
				toMove = tBar.getLastRing().getNum();
				break;
			case 'c':
				toMove = aBar.getLastRing().getNum();
				break;			
		}
		//Animating the movement from the 'From Bar'
		if (!fromMoveFlag)
		{
			toMoveFlag = 1;
			toPlaceFlag = 1;
			changeFlag = 0;
			int height = 3.0 * (rings+6);
			switch(f){
				case 'a': 
					if (zTrans < height - (3.0 * frmBar.getTotalRings()))
					{
						zTrans += 1.0;
					}
					else
					{
						fromMoveFlag = 1;
						toMoveFlag = 0;
					}
					break;
				case 'b': 
					if (zTrans < height - (3.0 * tBar.getTotalRings()))
					{
						zTrans += 1.0;
					}
					else
					{
						fromMoveFlag = 1;
						toMoveFlag = 0;
					}
					break;
				case 'c': 
					if (zTrans < height - (3.0 * aBar.getTotalRings()))
					{
						zTrans += 1.0;
					}
					else
					{
						fromMoveFlag = 1;
						toMoveFlag = 0;
					}
					break;
			}
		}
		//Animating the movement from the 'From bar' to the 'To Bar'
		if (!toMoveFlag)
		{
			//cout << "To Move Section" << endl;
			switch (f)
			{
			case 'a':
				switch (t)
				{
				case 'b':
					if (xTrans < 10.0 * rings)
						xTrans += 1;
					else
					{
						toMoveFlag = 1.0;
						toPlaceFlag = 0;
					}
					break;
				case 'c':
					if (xTrans < 2.0 * rings * 10.0)
						xTrans += 1.0;
					else
					{
						toMoveFlag = 1;
						toPlaceFlag = 0;
					}
					break;
				}
				break;
			case 'b':
				switch (t)
				{
				case 'a':
					if (xTrans > -(rings * 10.0))
						xTrans -= 1.0;
					else
					{
						toMoveFlag = 1;
						toPlaceFlag = 0;
					}
					break;
				case 'c':
					if (xTrans < rings * 10.0)
						xTrans += 1;
					else
					{
						toMoveFlag = 1.0;
						toPlaceFlag = 0;
					}
					break;
				}
			case 'c':
				switch (t)
				{
				case 'a':
					if (xTrans > -(2.0 * rings * 10.0))
						xTrans -= 1.0;
					else
					{
						toMoveFlag = 1;
						toPlaceFlag = 0;
					}
					break;
				case 'b':
					if (xTrans > - (rings * 10.0))
						xTrans -= 1.0;
					else
					{
						toMoveFlag = 1;
						toPlaceFlag = 0;
					}
					break;
				}
			}
		}
		//Animating the movement from the top of the 'To Bar' to where it must be placed.
		if (!toPlaceFlag)
		{
			switch (t)
			{
			case 'a':
				if (zTrans > 3.0 * frmBar.getTotalRings() + 2)
					zTrans -= 1.0;
				else
				{
					toPlaceFlag = 1;
					changeFlag = 1;
				}
				break;
			case 'b':
				if (zTrans > 3.0 * tBar.getTotalRings() + 2)
					zTrans -= 1.0;
				else
				{
					toPlaceFlag = 1;
					changeFlag = 1;
				}
				break;
			case 'c':
				if (zTrans > 3.0 * aBar.getTotalRings() + 2)
					zTrans -= 1.0;
				else
				{
					toPlaceFlag = 1;
					changeFlag = 1;
				}
				break;
			}
		}
		//After the animation has completed, Setting the current new positions of the disks.
		if (changeFlag)
		{
			switch (f)
			{
			case 'a':
				switch (t)
				{
				case 'b':
					tBar.addRing(frmBar.removeRing());
					break;
				case 'c':
					aBar.addRing(frmBar.removeRing());
					break;
				}
				break;
			case 'b':
				switch (t)
				{
				case 'a':
					frmBar.addRing(tBar.removeRing());
					break;
				case 'c':
					aBar.addRing(tBar.removeRing());
					break;
				}
				break;
			case 'c':
				switch (t)
				{
				case 'a':
					frmBar.addRing(aBar.removeRing());
					break;
				case 'b':
					tBar.addRing(aBar.removeRing());
					break;
				}
				break;
			}
			fromMoveFlag = 0;
			xTrans = 0.0;
			yTrans = 0.0;
			zTrans = 0.0;
			steps.pop_back();
			if (steps.empty())
				isAnimate = 0;
		}
		glutPostRedisplay();
		glutTimerFunc(animationPeriod, animate, 1);
	}
}
//Method to reset the whole scene so that the animation can begin from the Starting of the sequence
void reset(void){	
	while(steps.size()!= 0)
		steps.pop_back();
	while(frmBar.getTotalRings()!= 0)
		frmBar.removeRing();
	while(tBar.getTotalRings()!= 0)
		tBar.removeRing();
	while(aBar.getTotalRings()!= 0)
		aBar.removeRing();
	isAnimate = 0;
	xTrans = 0.0; yTrans = 0.0; zTrans = 0.0;
	for (int i = rings, j = 0; i > 0; i--, j++)
	{
		disks.push_back(Ring(3.0, 3.0 + i, j));
		frmBar.addRing(disks.back());
	}	
	towers(rings, 'a', 'b', 'c');
	steps.reverse();
}

// Initialization routine.
void setup(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST); // Enable depth testing.
	reset();
}

// OpenGL window reshape routine.
void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 16.0 / 9.0, 5.0, 400.0);

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
	case 'r':
		reset();
		glutPostRedisplay();
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
	default:
		break;
	}
}

// Callback routine for non-ASCII key entry.
void specialKeyInput(int key, int x, int y)
{
	if (key == GLUT_KEY_DOWN)
		animationPeriod += 1;
	if (key == GLUT_KEY_UP)
		if (animationPeriod > 1)
			animationPeriod -= 1;
	glutPostRedisplay();
}

// Routine to output interaction instructions to the C++ window.
void printInteraction(void)
{
	cout << "Interaction:" << endl;
	cout << "Press space to toggle between animation on and off." << endl
		 << "Press the up/down arrow keys to speed up/slow down animation." << endl
		 << "Press the r key to reset the program." << endl
		 << "Press the x, X, y, Y, z, Z keys to rotate the scene." << endl;
}

// Main routine.
int main(int argc, char **argv)
{
	printInteraction();
	glutInit(&argc, argv);

	glutInitContextVersion(2, 1);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(800, 450);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Towers of Hanoi");
	glutDisplayFunc(drawScene);
	glutReshapeFunc(resize);
	glutKeyboardFunc(keyInput);
	glutSpecialFunc(specialKeyInput);

	glewExperimental = GL_TRUE;
	glewInit();

	setup();

	glutMainLoop();
}
