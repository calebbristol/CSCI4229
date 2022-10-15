/*
Homework 2: Lorenz
Class: CSCI 4229 Computer Graphics
Author: Caleb Bristol

Credit to CSCI 4229 Teaching Team For Large Portion of Code,
modified by me to serve the Lorenz Attractor Homework


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

/*
All of the following variables are for use with the lorenz equations:
*/
double sigma = 5.0;
double beta = 3.0;
double rho = 60.0;
double scalar = 0.025; //Lorenz Scalar So It Fits In Window

int N = 20000;
double result[20000][3];



// Master Lorenz Equation, as pulled from wikipedia
/*
This function gets called and assigns values to the global variable result
It has to be called every time a variable of the lorenz equations is changed, to reassign the result
*/
void Lorenz()
{
    double dt = 0.001;

    // Initial Conditions
    double x = 1;
    double y = 1;
    double z = 1;

    double dx_dt;
    double dy_dt;
    double dz_dt;

    // Numerically Integrate Lorenz Function for N steps
    for (int i = 0; i < N; i++)
    {
        // Define Differential Equations
        dx_dt = sigma * (y - x);
        dy_dt = x * (rho - z) - y;
        dz_dt = x * y - beta * z;

        // Define New X,Y,Z values
        x += dx_dt*dt;
        y += dy_dt*dt;
        z += dz_dt*dt;

        // Write to Global Array
        result[i][0] = x*scalar;
        result[i][1] = y*scalar;
        result[i][2] = z*scalar;
    }
}

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
    // Call Lorenz Function
    Lorenz();

    // Erase window
    glClear(GL_COLOR_BUFFER_BIT);

    // Reset Transformations
    glLoadIdentity();

    // Set viewing angle
   glRotatef(ph,1,0,0);
   glRotatef(th,0,1,0);


   /* Draw Lorenz Integration */
   glColor3f(1,0,0);
   glBegin(GL_LINE_STRIP);
   for (int i=0;i<N;i++)
   {
       glVertex3d(result[i][0],result[i][1],result[i][2]);
   }
   glEnd();
   /*                         */


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
   Print("Angle=%d,%d  r=%.2f, b=%.2f, s=%.2f",th,ph,rho,beta,sigma);


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
   // Change Lorenz Variables
   else if (ch == 'q')
      sigma += 0.5;
   else if (ch == 'a')
      sigma -= 0.5;
   else if (ch == 'w')
      beta += 0.5;
   else if (ch == 's')
      beta -= 0.5;
   else if (ch == 'e')
      rho += 1.0;
   else if (ch == 'd')
      rho -= 1.0;
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
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);

	// Open with 300x300 window
	glutInitWindowSize(800,800);

	// Create window
	glutCreateWindow("Lorenz");

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
