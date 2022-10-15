/*
Homework 5: Lighting
Class: CSCI 4229 Computer Graphics
Author: Caleb Bristol

Credit to CSCI 4229 Teaching Team for Large Portion of Foundational Code

Key Bindings:
Arrow Keys: Look Around
W/A/S/D: Move Around (First Person Only)
E/Q: Move Up/Down (First Person Only)
PageUp/PageDown: Zoom In/Out (Overhead Only)
Esc: Exit Program
X: Toggle Axes
V: Toggle Viewing Mode (Orthogonal Overhead/Perspective Overhead/Perspective First Person)
+/-: FOV up/down

l: Toggle lighting
1/2: Decrease/Increase Ambient Light
3/4: Decrease/Increase Diffuse Light
5/6: Decrease/Increase Specular Light
7/8: Decrease/Increase Emitted Light
9/0: Decrease/Increase Shininess
F1: Toggle Smooth/Flat Shading
F2: Toggle Local Viewing Mode
F3: Toggle Light Distance (1/5)
o/O: Cycle Through Objects
m: Toggle Light Movement
<>: Move Light
[]: Lower/Raise Light


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
int axes=0;                     //  Display axes
int mode=0;                     //  Projection mode
int fp=0;                       //  Perspective First Person/Overhead
int th=0;                       //  Azimuth of view angle
int ph=0;                       //  Elevation of view angle
int fov=55;                     //  Field of view (for perspective)
double asp=1;                   //  Aspect ratio
double dim=5.0;                 //  Size of world
double camPos[3]={0.0,0.0,2.5}; //  Camera Position (For First Person)
int obj=1;                      //  Object Display Setting

// Light values
int move      =   1;  // Light Movement
int light     =   1;  // Lighting
int distance  =   5;  // Light distance
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  10;  // Ambient intensity (%)
int diffuse   =  50;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;  // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
 
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
 *  Set projection
 *  Called when window is updated (key press or reshape)
 */
static void Project()
{
   //  Tell OpenGL we want to manipulate the projection matrix
   glMatrixMode(GL_PROJECTION);
   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective transformation
   if (mode)
      gluPerspective(fov,asp,dim/4,4*dim);
   //  Orthogonal projection
   else
      glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
   //  Switch to manipulating the model matrix
   glMatrixMode(GL_MODELVIEW);
   //  Undo previous transformations
   glLoadIdentity();
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
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

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
   glNormal3f( 0, 0, 1);
   glVertex3f(-1,-1, 1);
   glVertex3f(+1,-1, 1);
   glVertex3f(+1,+1, 1);
   glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f( 1, 0, 0);
   glVertex3f(+1,-1,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,-1,+1);
   glVertex3f(-1,+1,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f( 0, 1, 0);
   glVertex3f(-1,+1,+1);
   glVertex3f(+1,+1,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f( 0,-1, 0);
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
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float black[] = {0,0,0,1};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   //  Offset
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);

   //  Angle Calculation for Sides
   float theta = atan(1-frat);

   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glColor3f(r,g,b);
   glNormal3f( 0, 0, 1);
   glVertex3f(-frat,-frat, 1);
   glVertex3f(+frat,-frat, 1);
   glVertex3f(+frat,+frat, 1);
   glVertex3f(-frat,+frat, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glVertex3f(+1,-1,-1);
   glVertex3f(-1,-1,-1);
   glVertex3f(-1,+1,-1);
   glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f( cos(theta), 0, sin(theta));
   glVertex3f(+frat,-frat,+1);
   glVertex3f(+1,-1,-1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+frat,+frat,+1);
   //  Left
   glNormal3f(-cos(theta), 0, sin(theta));
   glVertex3f(-1,-1,-1);
   glVertex3f(-frat,-frat,+1);
   glVertex3f(-frat,+frat,+1);
   glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f( 0, cos(theta), sin(theta));
   glVertex3f(-frat,+frat,+1);
   glVertex3f(+frat,+frat,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f( 0,-cos(theta), sin(theta));
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
   glNormal3f(0,0,-1);
   glVertex3f(-frat,-frat, 1);
   glVertex3f(+frat,-frat, 1);
   glVertex3f(+frat,-frat, 1);
   glVertex3f(+frat,+frat, 1);
   glVertex3f(+frat,+frat, 1);
   glVertex3f(-frat,+frat, 1);
   glVertex3f(-frat,+frat, 1);
   glVertex3f(-frat,-frat, 1);
   //  Back
   glNormal3f(0,0,1);
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
   glNormal3f(-1,0,0);
   glVertex3f(+frat,+frat,+1);
   glVertex3f(+1,+1,-1);
   glVertex3f(+frat,-frat,+1);
   glVertex3f(+1,-1,-1);
   glNormal3f(1,0,0);
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
    //  Set specular color to white
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    // Save Transformation
    glPushMatrix();
    // Offset
    glTranslated(x,y,z);
    glRotated(th,0,1,0);
    glScaled(dx,dy,dz);

    //  Angle Calculation for Sides
    float theta = atan(1-frat);

    // Sides
    glBegin(GL_QUADS);
    glColor3f(r,g,b);
    for(int i = 0; i <= 360; i+=15)
    {
        glNormal3f(-sin(theta),Cos(i),Sin(i));
        glVertex3f(1,Cos(i),Sin(i));
        glVertex3f(-1,frat*Cos(i),frat*Sin(i));
        glNormal3f(-sin(theta),Cos(i+15),Sin(i+15));
        glVertex3f(-1,frat*Cos(i+15),frat*Sin(i+15));
        glVertex3f(1,Cos(i+15),Sin(i+15));
    }
    glEnd();
    // Endcaps
    glBegin(GL_TRIANGLES);
    for(int i = 0; i <= 360; i+=15)
    {
        glNormal3f(1,0,0);
        glVertex3f(1,Cos(i),Sin(i));
        glVertex3f(1,Cos(i+15),Sin(i+15));
        glVertex3f(1,0,0);
        glNormal3f(-1,0,0);
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
         glNormal3f(-1,0,0);
         glVertex3f(1,Cos(i),Sin(i));
         glVertex3f(1,Cos(i+15),Sin(i+15));
         glNormal3f(1,0,0);
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
   glNormal3f(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
   glVertex3d(Sin(th)*Cos(ph) , Sin(ph) , Cos(th)*Cos(ph));
}



/*
 *  Draw a sphere 
 *     at (x,y,z)
 *     sized by dx,dy,dz
 *     of color r,g,b
 */
static void sphere(double x,double y,double z,
                    double dx, double dy, double dz,
                    double r, double g, double b)
{
   const int d=15;

   //  Save transformation
   glPushMatrix();
   //  Offset and scale
   glTranslated(x,y,z);
   glScaled(dx,dy,dz);

   glColor3f(r,g,b);
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

    //  Perspective - set eye position
    if (mode)
    {
       // First Person Perspective
       if (fp)
       {
           double Ex = camPos[0];
           double Ey = camPos[1];
           double Ez = camPos[2];
           gluLookAt(Ex,Ey,Ez,Ex+Sin(th)*Cos(ph),Ey+Sin(ph),Ez-Cos(th)*Cos(ph),0,Cos(ph),0);
       }
       // Overhead Perspective
       else
       {
           double Ex = -2*dim*Sin(th)*Cos(ph);
           double Ey = +2*dim        *Sin(ph);
           double Ez = +2*dim*Cos(th)*Cos(ph);
           gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
       }
    }
    //  Orthogonal - set world orientation
    else
    {
       glRotatef(ph,1,0,0);
       glRotatef(th,0,1,0);
    }

    //  Flat or smooth shading
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

    //  Light switch
    if (light)
    {
       //  Translate intensity to color vectors
       float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
       float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
       float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
       //  Light position
       float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
       //  Draw light position as ball (still no lighting here)
       glColor3f(1,1,1);
       sphere(Position[0],Position[1],Position[2], 0.1,0.1,0.1, 1,1,1);
       //  OpenGL should normalize normal vectors
       glEnable(GL_NORMALIZE);
       //  Enable lighting
       glEnable(GL_LIGHTING);
       //  Location of viewer for specular calculations
       glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
       //  glColor sets ambient and diffuse color materials
       glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
       glEnable(GL_COLOR_MATERIAL);
       //  Enable light 0
       glEnable(GL_LIGHT0);
       //  Set ambient, diffuse, specular components and position of light 0
       glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
       glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
       glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
       glLightfv(GL_LIGHT0,GL_POSITION,Position);
    }
    else
       glDisable(GL_LIGHTING);

    if (obj==1)
       sphere(0,0,0,0.5,0.5,0.5,0.5,0.5,1.0);
    else if (obj==2)
       cube(0,0,0,1,1,1,1,0.55,0.55,0);
    else if (obj==3)
       Cylinder(0,0,0,1,1,1,1,0.5,0.5,0,1);
    else if (obj==4)
       Cylinder(0,0,0,1,1,1,1,0.5,0.5,0,0);
    else if (obj==5)
    {
       Cylinder(0,0,0,1,1,1,1,0.5,0.5,0,1);
       Cylinder_outline(0,0,0,1,1,1,1,1,1,0,1);
    }
    else
    {
    // Draw Planet
    sphere(0,0,0,0.5,0.5,0.5,0.5,0.5,1.0);
    // Draw Spaceships
    Spaceship(0.1,0,1,0.02,0.02,0.02,0);
    Spaceship(0.1,0,1.3,0.02,0.02,0.02,0);
    Spaceship(0.4,0,1.05,0.02,0.02,0.02,-10);
    Spaceship(0.35,0,1.35,0.02,0.02,0.02,10);
    Spaceship(0,0.3,1,0.05,0.05,0.05,0);
    Spaceship(0,0,-1.5,0.1,0.1,0.1,180);
    }

    /* Draw Axes */
    //  White
    glColor3f(1,1,1);
    if (axes)
    {
        const double len=1.5;  //  Length of axes
        glDisable(GL_LIGHTING);
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
    }
    /*           */


    /* Text On Window Display */
    //  Five pixels from the lower left corner of the window
    glWindowPos2i(5,5);
    //  Print the text string
    Print("Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s",th,ph,dim,fov,mode?"Perpective":"Orthogonal", light?"On":"Off");
    if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",smooth?"Smooth":"Flat",local?"On":"Off",distance,ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }


    //  Render the scene
    ErrCheck("display");
    glFlush();
    glutSwapBuffers();
}

  

// GLUT calls idle while idle
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
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
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_UP)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
      dim -= 0.1;
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project();
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}



// GLUT calls key when a key input is registered
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Switch display mode
   else if (ch == 'v' || ch == 'V')
   {
      if (mode)
      {
          fp = 1-fp;
          mode = fp;
      }
      else
      {
          mode = 1;
          fp = 0;
      }
   }
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   else if (ch == 'w')
   {
      camPos[0] += 0.25*Sin(th);
      camPos[2] -= 0.25*Cos(th);
   }
   else if (ch == 'a')
   {
      camPos[0] -= 0.25*Cos(th);
      camPos[2] -= 0.25*Sin(th);
   }
   else if (ch == 's')
   {
      camPos[0] -= 0.25*Sin(th);
      camPos[2] += 0.25*Cos(th);
   }
   else if (ch == 'd')
   {
      camPos[0] += 0.25*Cos(th);
      camPos[2] += 0.25*Sin(th);
   }
   else if (ch == 'e')
      camPos[1] += 0.25;
   else if (ch == 'q')
      camPos[1] -= 0.25;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='1' && ambient>0)
      ambient -= 5;
   else if (ch=='2' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='3' && diffuse>0)
      diffuse -= 5;
   else if (ch=='4' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='5' && specular>0)
      specular -= 5;
   else if (ch=='6' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='7' && emission>0)
      emission -= 5;
   else if (ch=='8' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='9' && shininess>-1)
      shininess -= 1;
   else if (ch=='0' && shininess<7)
      shininess += 1;
   //  Switch scene/object
   else if (ch == 'o')
      obj = (obj+1)%6;
   else if (ch == 'O')
      obj = (obj+5)%6;
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project();
   //  Animate if requested
   glutIdleFunc(move?idle:NULL);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}



// GLUT calls reshape when window is resized
void reshape(int width, int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, RES*width,RES*height);
   //  Set projection
   Project();
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

    // Idle function
    glutIdleFunc(idle);

    // Start GLUT
    glutMainLoop();

    return 0;
}