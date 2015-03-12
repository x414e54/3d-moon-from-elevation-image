This is a quick application I created that takes in a gray-scale elevation image and then procedurally produces a sphere and alters the vertexes to map the elevation data. This produces a 3D model of the moon.

This has then been extended to render only current visible segments of the moon, ready to simulate walking on the moon.

This requires an opengl, sdl and sdl image.

Compiled using //g++ moon.cpp -lGL -lSDL -lGLU -lSDL_image

g++ segmentmoon.cpp freeglutGeometry.cpp -lGL -lSDL -lGLU -lSDL_image -o segmentmoon
g++ moon.cpp freeglutGeometry.cpp -lGL -lSDL -lGLU -lSDL_image -o moon

run using 

./moon WAC_GLD100_E000N1800_004P.TIF

This is still a prototype.
