# EXAM_askelej

## Content
This repo contains source (.h and .cpp files),
as well as shader files and assets such as texture
and model objects.

## Setup

They way I set up this project was to use cMake from the Lab02.
I linked it with assimp library for model loading, and stb_image for 
loading images. I got a ton of help from a shaderloader and objectloader from www.learnopengl.com

## How I worked

The first thing I did was to try to reuse the Pacman project.
I could not reuse everything, and the framework was weak.
So I search for some useful resources 
to strengthen my framework, as I did not think I had much time.

I began by looking at various way to load the heightmap provided
to turn it into a vertex array that I could display on the screen.
This task turned out to be very difficult, and I found I was running 
out of time. To solve this temporarily, I opened up Blender, and 
created a model based on the heightmap, then load it in my project.

I then did the things I found I had most time to do. So I decided 
to get the snow, grass and water ready, as well as lighting.
I was able to get the snow and water altitudes, but they do not
interpolate at all, which was a requirement. 

After loading the plane, I started applying some attitude controls
to it. It was very tricky to make it behave like a real aircraft,
but I was able to get it to roll (left and right bank), and pitch up and down.
Yaw (rudder), however, is not included. 

Whilst having accomplished this so far, I thought it was time to add the functionality
to change the seasons. The seasons do not change overtime, since I haden't had time to
figure out how to kep track of days. One method would be to count the times the sun has orbited
and change the season index accordingly.

I added orbit to the lighting by using some tricky trigonometry. That worked nicely for me.
This is the same method used when I implemented the camera orbit around the aircraft.

I then applied the same properties to the aircraft as for the camera movement,
sothat it could follow the aircraft. I also let it be optional that 
the user toggle from freecamera mode, to ground camera with plane locked-mode,
and spot-view (following the plane and orbit the camera around it).

In the final stages I implemented airspeed controll, however, the plane doesnt stall,
nor does it lose any lift when the airspeed is reduced. 

I have focused more on Task II than on Task I. 
This project does not count days, months nor does it keep track of seasons, simply because I prioritized
the tasks I thought I could get the most out of, and dealing with the other tasks if there was any time left.

## How to play

To control the plane, press "," (comma key) to increase thrust (on a glider?)
and "." (period) to deploy the speedbrakes. The airspeed will drop.
Press IJKL to maneuver the glider. JL to bank left or right, and IK to pitch down or up.

To control the camera, press F to toggle between spot-view, ground-view, and free camera-view.
In spot-view use Z and X keys to orbit the camera around left or right.
In free camera-view, use WASD and mouse to move toward any mouse direction, left right or backwards.

To change seasons press 1 for Spring, 2 for Summer, 3 for Autumn, and 4 for Winter.
Please note that the water also goes down as it freezes to ice/ tides.

During midflight, you have to possibility to save any of your positions in realtime by pressing T
and then move to those positions by pressing G.
To reset to first position press R



## Bugs

As you may or may not have noticed, the plane functions perfectly in one direction, but loses its
sense of "plane" control once it has a new heading.










