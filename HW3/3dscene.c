/*
Homework 3: 3D Scene
Class: CSCI 4229 Computer Graphics
Author: Caleb Bristol

Credit to CSCI 4229 Teaching Team for Large Portion of Foundational Code

Key Bindings:



*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#ifdef USEGLEW
#include <GL/glew.h>
#endif
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
// Tell Xcode IDE to not gripe about OpenGL deprecation
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#else
#include <GL/glut.h>
#endif
//  Default resolution
//  For Retina displays compile with -DRES=2
#ifndef RES
#define RES 1
#endif

// Global Variables
int th = 0;         // Azimuth of view angle
int ph = 0;         // Elevation of view angle
double z = 0;       // Z Variable

//  Cosine and Sine in degrees
#define Cos(x) (cos((x)*3.14159265/180))
#define Sin(x) (sin((x)*3.14159265/180))

#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
   char    buf[LEN];
   char*   ch=buf;
   va_list args;
   //  Turn the parameters into a character string
   va_start(args,format);
   vsnprintf(buf,LEN,format,args);
   va_end(args);
   //  Display the characters one at a time at the current raster position
   while (*ch)
      glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Print message to stderr and exit
 */
void Fatal(const char* format , ...)
{
   va_list args;
   va_start(args,format);
   vfprintf(stderr,format,args);
   va_end(args);
   exit(1);
}

/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimensions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 float r, float g, float b,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(r,g,b);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(-1,-1,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw the outline of a cube
 *      following same principles as cube()
 *      except with lines, drawing 8 lines instead of 6 faces
 */

static void cube_outline(double x,double y,double z,
                 double dx,double dy,double dz,
                 float r, float g, float b,
                 double th)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_LINES);
   //  Front
   glColor3f(r,g,b);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   glVertex3f(-1,+1, 1);
   glVertex3f(-1,-1, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,-1,-1);
   //  Sides
   glColor3f(r,g,b);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,-1,-1);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a cuboid pyramid
 *      at x,y,z
 *      with scale dx,dy,dz
 *      color r,g,b
 *      rotation th
 *      ratio between front/back face frat
 *
 */

static void cubamid(double x,double y,double z,
                 double dx,double dy,double dz,
                 float r, float g, float b,
                 double th, 
                 float frat)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(r,g,b);
   glVertex3f(-frat,-frat, 1);
   glVertex3f(+frat,-frat, 1);
   glVertex3f(+frat,+frat, 1);
   glVertex3f(-frat,+frat, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glVertex3f(+frat,-frat,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+frat,+frat,+1);
   //  Left
   glVertex3f(-1,-1,-1);
   glVertex3f(-frat,-frat,+1);
   glVertex3f(-frat,+frat,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glVertex3f(-frat,+frat,+1);
   glVertex3f(+frat,+frat,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glVertex3f(-1,-1,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+frat,-frat,+1);
   glVertex3f(-frat,-frat,+1);
   //  End
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw the outline of a cubamid
 *      following same principles as cubamid()
 *      except with lines, drawing 8 lines instead of 6 faces
 */

static void cubamid_outline(double x,double y,double z,
                 double dx,double dy,double dz,
                 float r, float g, float b,
                 double th,
                 float frat)
{
   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_LINES);
   //  Front
   glColor3f(r,g,b);
   glVertex3f(-frat,-frat, 1);
   glVertex3f(+frat,-frat, 1);
   glVertex3f(+frat,-frat, 1);
   glVertex3f(+frat,+frat, 1);
   glVertex3f(+frat,+frat, 1);
   glVertex3f(-frat,+frat, 1);
   glVertex3f(-frat,+frat, 1);
   glVertex3f(-frat,-frat, 1);
   //  Back
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,-1,-1);
   //  Sides
   glColor3f(r,g,b);
   glVertex3f(+frat,+frat,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+frat,-frat,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-frat,+frat,+1);
   glVertex3f(-1,+1,-1);
   glVertex3f(-frat,-frat,+1);
   glVertex3f(-1,-1,-1);
   glEnd();
   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draws a Cylinder    
 *          at x,y,z
 *          of color r,g,b
 *          with scale dx,dy,dz
 *          and ratio between front/back face of frat
 *
 */
 static void Cylinder(double x, double y, double z,
                     double dx, double dy, double dz,
                     float r, float g, float b,
                     double th,
                     float frat)
 {
     // Save Transformation
     glPushMatrix();

     // Offset
     glTranslated(x,y,z);
     glRotated(th,0,1,0);
     glScaled(dx,dy,dz);

     // Sides
     glBegin(GL_QUADS);
     glColor3f(r,g,b);

     for(int i = 0; i <= 360; i+=15)
     {
         glVertex3f(1,Cos(i),Sin(i));
         glVertex3f(-1,frat*Cos(i),frat*Sin(i));
         glVertex3f(-1,frat*Cos(i+15),frat*Sin(i+15));
         glVertex3f(1,Cos(i+15),Sin(i+15));
     }
     glEnd();

     // Endcaps
     glBegin(GL_TRIANGLES);

     for(int i = 0; i <= 360; i+=15)
     {
         glVertex3f(1,Cos(i),Sin(i));
         glVertex3f(1,Cos(i+15),Sin(i+15));
         glVertex3f(1,0,0);
         glVertex3f(-1,frat*Cos(i),frat*Sin(i));
         glVertex3f(-1,frat*Cos(i+15),frat*Sin(i+15));
         glVertex3f(-1,0,0);
     }
     glEnd();


     // Undo Transformation
     glPopMatrix();
 }

 /*
 *  Draws a Cylinder Outline   
 *          at x,y,z
 *          of color r,g,b
 *          with scale dx,dy,dz
 *          and ratio between front/back face of frat
 *
 *  Only Covers the endcaps
 */
 static void Cylinder_outline(double x, double y, double z,
                     double dx, double dy, double dz,
                     float r, float g, float b,
                     double th,
                     float frat)
 {
     // Save Transformation
     glPushMatrix();

     // Offset
     glTranslated(x,y,z);
     glRotated(th,0,1,0);
     glScaled(dx,dy,dz);

     // Sides
     glBegin(GL_LINES);
     glColor3f(r,g,b);

     for(int i = 0; i <= 360; i+=15)
     {
         glVertex3f(1,Cos(i),Sin(i));
         glVertex3f(1,Cos(i+15),Sin(i+15));
         glVertex3f(-1,frat*Cos(i),frat*Sin(i));
         glVertex3f(-1,frat*Cos(i+15),frat*Sin(i+15));
     }
     glEnd();


     // Undo Transformation
     glPopMatrix();
 }

 /*
 *  Draw vertex in polar coordinates
 */
static void Vertex(double th,double ph)
{
   glColor3f(Cos(th)*Cos(th) , Sin(ph)*Sin(ph) , Sin(th)*Sin(th));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}

 /*
 *  Draw a sphere (version 2)
 *     at (x,y,z)
 *     radius (r)
 */
static void sphere2(double x,double y,double z,double r)
{
   const int d=15;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(r,r,r);

   //  Latitude bands
   for (int ph=-90;ph<90;ph+=d)
   {
      glBegin(GL_QUAD_STRIP);
      for (int th=0;th<=360;th+=d)
      {
         Vertex(th,ph);
         Vertex(th,ph+d);
      }
      glEnd();
   }

   //  Undo transformations
   glPopMatrix();
}

/*
 *  Draw a Spaceship
 */
 static void Spaceship(double x, double y, double z,
                       double dx, double dy, double dz,
                       double th)
 {
     // Save previous Transformations
     glPushMatrix();

     // Transform 
     glTranslated(x,y,z);
     glRotated(th,0,1,0);
     glScaled(dx,dy,dz);

     // Fuselage
     cube(x,y,z,2,0.5,1,1,0.5,0.5,0);
     cube_outline(x,y,z,2,0.5,1,1,1,1,0);

     // Not A Gun Mount
     cube(x,y+0.75,z,1.7,0.25,0.7,1,0.55,0.55,0);
     cube_outline(x,y+0.75,z,1.7,0.25,0.7,1,1,1,0);

     cube(x-1.6,y+1,z,0.3,1,0.5,1,0.6,0.6,0);
     cube_outline(x-1.6,y+1,z,0.3,1,0.5,1,1,1,0);

     // Cockpit
     Cylinder(x+2.25,y,z,0.25,0.5,0.9,0.5,0.5,1,180,0.5);
     Cylinder_outline(x+2.25,y,z,0.25,0.5,0.9,0,0,0,180,0.5);

     // Left Wing
     cubamid(x,y,z-3,2,0.5,2,1,0,0.5,180,0.5);
     cubamid_outline(x,y,z-3,2,0.5,2,1,1,1,180,0.5);

     // Right Wing
     cubamid(x,y,z+3,2,0.5,2,1,0,0.5,0,0.5);
     cubamid_outline(x,y,z+3,2,0.5,2,1,1,1,0,0.5);

     // Thrusters
     Cylinder(x-2.5,y,z+0.5,0.5,0.5,0.5,0.5,0.5,0.5,180,0.5);
     Cylinder_outline(x-2.5,y,z+0.5,0.5,0.5,0.5,1,1,1,180,0.5);
     Cylinder(x-2.5,y,z-0.5,0.5,0.5,0.5,0.5,0.5,0.5,180,0.5);
     Cylinder_outline(x-2.5,y,z-0.5,0.5,0.5,0.5,1,1,1,180,0.5);

     // Thrusters (Wingtip edn.)
     Cylinder(x-1.45,y,z+4.25,0.3,0.3,0.3,0.5,0.5,0.5,180,0.5);
     Cylinder_outline(x-1.45,y,z+4.25,0.3,0.3,0.3,1,1,1,180,0.5);
     Cylinder(x-1.45,y,z-4.25,0.3,0.3,0.3,0.5,0.5,0.5,180,0.5);
     Cylinder_outline(x-1.45,y,z-4.25,0.3,0.3,0.3,1,1,1,180,0.5);

     Cylinder(x,y,z+4.25,1.4,0.5,0.5,0.5,0.5,0.5,180,1);
     Cylinder_outline(x,y,z+4.25,1.4,0.5,0.5,1,1,1,180,1);
     Cylinder(x,y,z-4.25,1.4,0.5,0.5,0.5,0.5,0.5,180,1);
     Cylinder_outline(x,y,z-4.25,1.4,0.5,0.5,1,1,1,180,1);

     //Thrust
     Cylinder(x-3.5,y,z+0.5,0.5,0.45,0.45,1,0.7,0.7,0,0);
     Cylinder(x-3.5,y,z-0.5,0.5,0.45,0.45,1,0.7,0.7,0,0);

     Cylinder(x-3.75,y,z+0.5,0.75,0.4,0.4,1,0.6,0.6,0,0);
     Cylinder(x-3.75,y,z-0.5,0.75,0.4,0.4,1,0.6,0.6,0,0);

     //Thrust (Wingtip edn.)
     Cylinder(x-2.05,y,z+4.25,0.3,0.3,0.3,1,0.7,0.7,0,0);
     Cylinder(x-2.05,y,z-4.25,0.3,0.3,0.3,1,0.7,0.7,0,0);


     // Definitely Not Guns ;)
     Cylinder(x+0.6,y+1.5,z+0.25,2.4,0.1,0.1,0.5,0.5,0.5,0,1);
     Cylinder_outline(x+0.6,y+1.5,z+0.25,2.4,0.1,0.1,1,1,1,0,1);

     Cylinder(x-0.6,y+1.5,z+0.25,1.2,0.2,0.2,0.2,0.2,0.2,0,1);
     Cylinder_outline(x-0.6,y+1.5,z+0.25,1.2,0.2,0.2,1,1,1,0,1);

     Cylinder(x,y+1.5,z-0.25,1.8,0.1,0.1,0.5,0.5,0.5,0,1);
     Cylinder_outline(x,y+1.5,z-0.25,1.8,0.1,0.1,1,0,0,0,1);

     // Definitely Not Guns (Wingtip edn.)
     Cylinder(x+1.75,y,z+4.25,0.8,0.1,0.1,0.5,0.5,0.5,180,1);
     Cylinder_outline(x+1.75,y,z+4.25,0.8,0.1,0.1,1,0,0,180,1);

     Cylinder(x+1.75,y,z-4.25,0.8,0.1,0.1,0.5,0.5,0.5,180,1);
     Cylinder_outline(x+1.75,y,z-4.25,0.8,0.1,0.1,1,0,0,180,1);


     // Finish Drawing

     // Undo transformations
     glPopMatrix();
 }

/*
 *  Check for OpenGL errors
 */
void ErrCheck(const char* where)
{
   int err = glGetError();
   if (err) fprintf(stderr,"ERROR: %s [%s]\n",gluErrorString(err),where);
}

// Display Function
void display()
{

    //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

    // Reset Transformations
    glLoadIdentity();

    // Set viewing angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);

   // Draw Planet
   sphere2(0,0,0,0.5);

   // Draw Spaceships
   Spaceship(0.1,0,1,0.02,0.02,0.02,0);
   Spaceship(0.1,0,1.3,0.02,0.02,0.02,0);
   Spaceship(0.4,0,1.05,0.02,0.02,0.02,-10);
   Spaceship(0.35,0,1.35,0.02,0.02,0.02,10);

   Spaceship(0,0.3,1,0.05,0.05,0.05,0);

   Spaceship(0,0,-1.5,0.1,0.1,0.1,180);

   /* Draw Axes */
   //  White
   glColor3f(1,1,1);
   const double len=1.5;  //  Length of axes
   glBegin(GL_LINES);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(len,0.0,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,len,0.0);
   glVertex3d(0.0,0.0,0.0);
   glVertex3d(0.0,0.0,len);
   glEnd();
   //  Label axes
   glRasterPos3d(len,0.0,0.0);
   Print("X");
   glRasterPos3d(0.0,len,0.0);
   Print("Y");
   glRasterPos3d(0.0,0.0,len);
   Print("Z");
   /*           */

   /* Text On Window Display */
   //  Five pixels from the lower left corner of the window
   glWindowPos2i(5,5);
   //  Print the text string
   //Print("Angle=%d,%d  r=%.2f, b=%.2f, s=%.2f",th,ph,rho,beta,sigma);


   //  Render the scene
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();

}

// GLUT calls special when a non ascii keyboard input is registered
void special(int key, int x, int y)
{
    //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// GLUT calls key when a key input is registered
void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

// GLUT calls reshape when window is resized
void reshape(int width, int height)
{
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Orthogonal projection
   const double dim=2.5;
   double asp = (height>0) ? (double)width/height : 1;
   glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
}

// Main Function, Called On Startup
int main(int argc,char* argv[])
{
	// Initialize GLUT with user input
	glutInit(&argc,argv);

	// Initialize Color, Double Buffer
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

	// Open with 300x300 window
	glutInitWindowSize(800,800);

	// Create window
	glutCreateWindow("Caleb Bristol");

	#ifdef USEGLEW
    //  Initialize GLEW
    if (glewInit()!=GLEW_OK) Fatal("Error initializing GLEW\n");
    #endif

    // Initialize display as glut Display Function
    glutDisplayFunc(display);

    // Call reshape when window resized
    glutReshapeFunc(reshape);

    // Call special when special character is pressed
    glutSpecialFunc(special);

    // Call key when key is pressed
    glutKeyboardFunc(key);

    // Start GLUT
    glutMainLoop();

    return 0;
}
