Homework 5: Lighting
Author: Caleb Bristol
Credit to CSCI 4229 Teaching Team for Large Portion of Code

///////////////////////////////////////////////////////////
Key Bindings:
Arrow Keys: Look Around
W/A/S/D: Move Around (First Person Only)
E/Q: Move Up/Down (First Person Only)
PageUp/PageDown: Zoom In/Out (Overhead Only)
Esc: Exit Program
X: Toggle Axes
V: Toggle Display Mode (Orthogonal Overhead/Perspective Overhead/Perspective First Person)
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

////////////////////////////////////////////////////////////
This project took about 4 hours to complete. I can't focus so probably more like 3.
The lighting on the outlines of the shapes IS WRONG.
I didn't assign normals to the shape outline functions at first, so it took the last assigned normal for lighting.
This meant that all of the outlines would light up when the light source was on one side of the object.
It happened to be the wrong side.
I thought it looked cool so I decided to code in the opposite normals for the outlines.
This way they would light up in response to darkness, I thought it looked kinda sci-fi so I kept it.
If I want to change it I only have to change the sign of the normal vectors anyways so no big deal.

A large portion of code was taken from example 13.
The extent of my work was converting it to suit my program (updated from homework 4) as well as computing the
normals for my shapes. I won't pretend like it was that hard to compute the normals, but I did have to write the
shapes down on paper so it wasn't easy either. I used trigonometry instead of cross products based on the 
recommendation that resizing the scene will resize the normal vector, and it worked very well with the way I 
coded my object definitions, so I will continue using this strategy as long as it works.