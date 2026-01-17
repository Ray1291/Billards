## Billiards V1.0

A proof of concept game of billards built using OpenGL/GLUT in C.

This project consists of the following C source files:
- main.c
- movement_handling.c
- drawing.c
- collisions.c
- arraylist.c

And contains one third party single header file library
- stb_image.h - used to load textures

# Features:

- **Custom Dynamic Array (`arraylist.c`)**:  
  Implements a lightweight, resizable array used to manage all balls on the table. This allows fast insertion, deletion, and random access, making **collision detection and physics calculations more efficient** during gameplay.

- **Configurable Game Parameters (`pool.h`)**:  
  Users can customize ball size, number of balls, friction, and other gameplay settings through a single header file.

# Installation:


Before compiling, make sure the following libraries are installed on your system:

- **OpenGL** (mesa or your GPU drivers)
- **GLUT** (FreeGLUT recommended)
- **GLEW** (OpenGL Extension Wrangler)
- **GLU** (OpenGL Utility Library)
- **C standard libraries** (usually included by default)

Clone the repo

` git clone https://github.com/Ray1291/Billiards`

Navigate to the directory

` cd "YOUR_DIRECTORY"`

compile the program

` gcc main.c arraylist.c drawing.c movement_handling.c collisions.c -o a.out -lglut -lGLEW -lGL -lGLU -lm -g `

Run it

` ./run.out`

# Usage
Upon running the program, a window will pop up with the balls setup ready for play. Just click and drag on the cue ball to fire it off.

# Contributing 
Feel free to submit issues or pull requests. Contributions to improve the code or add features are welcome!

# Future Feature Plans
- Start Menu
- Adding text drawing support using gltext.h
- Power bar to show shot power
- Sound Effects
- Creation of a CMAKE file for use cross platform
- Menu to easily change game parameters (number of balls, strength of cue, etc...)
- Game modes (8-ball, 9-ball, Straight Pool)
